#pragma once

#include <librealsense2/rs.hpp> 
#include <librealsense2/rsutil.h>
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
	void load(string path);

	void enableColor(int width, int height, int fps = 60);
	void enableIr(int width, int height, int fps = 60);
	void enableDepth(int width, int height, int fps = 60);
	void threadedFunction();
	void update();

	float getDistanceAt(int x, int y);
	glm::vec3 getWorldCoordinateAt(float x, float y);

	ofxGuiGroup* setupGUI();
	void onD400BoolParamChanged(bool &value);
	void onD400IntParamChanged(int &value);
	void onD400ColorizerParamChanged(float &value);
	ofxGuiGroup* getGui();


	void setAligned(bool aligned)
	{
		if (aligned && !this->bAligned)
		{
			if (depth_enabled)
			{
				depth_texture->allocate(color_width, color_height, GL_RGB);
				raw_depth_texture->allocate(color_width, color_height, GL_R16);
				depth_width = color_width;
				depth_height = color_height;
			}
		}
		this->bAligned = aligned;
	}

	void drawDepthRaw(int x, int y)
	{
		if (depth_enabled)
			raw_depth_texture->draw(x, y);
	}

	void drawDepth(int x, int y)
	{
		if(depth_enabled)
			depth_texture->draw(x, y);
	}

	void drawColor(int x, int y)
	{
		if (color_enabled)
			color_texture->draw(x, y);
	}

	void drawIR(int x, int y)
	{
		if (ir_enabled)
			ir_tex->draw(x, y);
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
		if (bAligned)
		{
			rs2::frame color;
			rs2video_queue.poll_for_frame(&color);
			return color.as<rs2::video_frame>().get_width();
		}
		return color_width;
	}

	float getColorHeight()
	{
		if (bAligned)
		{
			rs2::frame color;
			rs2video_queue.poll_for_frame(&color);
			return color.as<rs2::video_frame>().get_height();
		}
		return color_height;
	}

	bool isFrameNew()
	{
		return bFrameNew;
	}

	shared_ptr<ofTexture> getDepthTexture()
	{
		return depth_texture;
	}

	shared_ptr<ofTexture> getDepthRawTexture()
	{
		return raw_depth_texture;
	}

	shared_ptr<ofTexture> getColorTexture()
	{
		return color_texture;
	}

	float getDepthScale()
	{
		return depthScale;
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


	rs2::frame _depth;

	//attribute
	ofxGuiGroup     _D400Params;
	ofxToggle       _autoExposure;
	ofxToggle       _enableEmitter;
	ofxIntSlider    _irExposure;
	ofxFloatSlider  _depthMin;
	ofxFloatSlider  _depthMax;

	ofPixels         _colBuff, _irBuff, _depthBuff;
	ofShortPixels    _rawDepthBuff;

	float depthScale;

	int deviceId;
	string device_serial;
	bool color_enabled = false, ir_enabled = false, depth_enabled = false;
	atomic_bool _isFrameNew;
	bool bFrameNew = false;

	int color_width, color_height;
	int ir_width, ir_height;
	int depth_width, depth_height;

	bool bReadFile = false;
	bool bAligned = false;

	ofPtr<ofTexture> depth_texture, raw_depth_texture, color_texture, ir_tex;


	void start()
	{
		startThread();
	}

	void stop()
	{
		stopThread();
	}

	float calcDepthScale(rs2::device dev)
	{
		// Go over the device's sensors
		for (rs2::sensor& sensor : dev.query_sensors())
		{
			// Check if the sensor if a depth sensor
			if (rs2::depth_sensor dpt = sensor.as<rs2::depth_sensor>())
			{
				return dpt.get_depth_scale();
			}
		}
		throw std::runtime_error("Device does not have a depth sensor");
	}
};
