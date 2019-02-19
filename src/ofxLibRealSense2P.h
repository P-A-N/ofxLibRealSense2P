#pragma once

#include <librealsense2/rs.hpp> 
#include "ofMain.h"
#include "ofxGui.h"

class ofxLibRealSense2P : public ofThread
{

public:
	ofxLibRealSense2P()
	{
	}
	~ofxLibRealSense2P()
	{
		stop();
		waitForThread(false);
	}

	void setupDevice(int deviceID = 0);

	void enableColor(int width, int height, int fps = 60);
	void enableIr(int width, int height, int fps = 60);
	void enableDepth(int width, int height, int fps = 60);
	void threadedFunction();
	void update();

	float getDistanceAt(int x, int y);

	ofxGuiGroup* setupGUI();
	void onD400BoolParamChanged(bool &value);
	void onD400IntParamChanged(int &value);
	void onD400ColorizerParamChanged(float &value);
	ofxGuiGroup* getGui();

	void drawDepthRaw()
	{
		raw_depth_texture.draw(0, 0);
	}

	void drawDepth()
	{
		depth_texture.draw(0, 0);
	}

	void drawColor()
	{
		col_tex.draw(0, 0);
	}

	void drawIR()
	{
		ir_tex.draw(0, 0);
	}

	float getDepthWidth()
	{
		return depth_width;
	}

	float getDepthHeight()
	{
		return depth_height;
	}

	float getColorWidth()
	{
		return color_width;
	}

	float getColorHeight()
	{
		return color_height;
	}

	bool isFrameNew()
	{
		return bFrameNew;
	}

	void startStream();

private:
	ofThreadChannel<rs2::frameset> frameChannel;
	rs2::device		rs2device;
	rs2::config		rs2config; 
	rs2::colorizer  rs2colorizer;
	rs2::pipeline	rs2_pipeline;

	rs2::frame_queue rs2depth_queue;
	rs2::frame_queue rs2video_queue;
	rs2::frame_queue rs2ir_queue;

	//attribute
	ofxGuiGroup     _D400Params;
	ofxToggle       _autoExposure;
	ofxToggle       _enableEmitter;
	ofxIntSlider    _irExposure;
	ofxFloatSlider  _depthMin;
	ofxFloatSlider  _depthMax;

	uint8_t         *_colBuff, *_irBuff, *_depthBuff;
	uint16_t        *_rawDepthBuff;

	int deviceId;
	string device_serial;
	bool color_enabled = false, ir_enabled = false, depth_enabled = false;
	atomic_bool _isFrameNew;
	bool bFrameNew = false;

	int color_width, color_height;
	int ir_width, ir_height;
	int depth_width, depth_height;

	ofTexture depth_texture, raw_depth_texture, col_tex, ir_tex;


	void start()
	{
		startThread();
	}

	void stop()
	{
		stopThread();
	}
};
