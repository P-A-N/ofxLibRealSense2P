#pragma once

#include <librealsense2/rs.hpp> 
#include <librealsense2/rsutil.h>
#include "ofMain.h"
#include "ofxLRS2/Filter.h"
#include "ofxLRS2/Rs2Config.h"

class ofxLibRealSense2P : public ofThread
{
	friend class GuiUtils;
public:
	using nano_sec = ratio<1, 1000000000>;
	using mill_sec = ratio<1, 1000>;
	using sec = ratio<1, 1>;

	//color schema for changing depth representation
	enum COLOR_SCHEMA
	{
		COLOR_SCHEMA_Jet,
		COLOR_SCHEMA_Classic, 
		COLOR_SCHEMA_WhiteToBlack, 
		COLOR_SCHEMA_BlackToWhite, 
		COLOR_SCHEMA_Bio, 
		COLOR_SCHEMA_Cold, 
		COLOR_SCHEMA_Warm, 
		COLOR_SCHEMA_Quantized, 
		COLOR_SCHEMA_Pattern
	};

	ofxLibRealSense2P()
	{
	}
	~ofxLibRealSense2P()
	{
		stop();
		waitForThread(false);
	}

	void setupDevice(int deviceID = 0, bool listAvailableStream = true);
	void startStream(bool threaded = true);
	void stopStream();

	//depth postprocess filter
	void setupFilter();

	//Stream setting
	void enableColor(int width, int height, int fps = 60, bool useArbTex = true);
	void enableIr(int width, int height, int fps = 60, bool useArbTex = true);
	void enableDepth(int width, int height, int fps = 60, bool useArbTex = true);
	
	//record 
	void startRecord(string path);
	void stopRecord();
	void playbackRecorded();
	bool isRecording();
	void load(string path);
	bool isPlayback() const;
	void setPosition(double position);
	double getPosition() const;
	template<typename type>
	double getDurationAs() const
	{
		if (this->isPlayback())
		{
			rs2::playback playback = rs2device.as<rs2::playback>();
			auto duration = std::chrono::duration_cast<std::chrono::duration<double, type>>(playback.get_duration());
			return std::chrono::duration_cast<std::chrono::duration<double, type>>(duration).count();
		}
		else
		{
			ofLogWarning() << "getDurationAs() : it can be used in playbacking bag only.";
		}
	}


	void threadedFunction();
	void update();

	float getDistanceAt(int x, int y);
	glm::vec3 getWorldCoordinateAt(float x, float y);

	//make frames size aligned
	void setAligned(bool aligned)
	{
		if (depth_enabled && color_enabled)
		{
			this->bAligned = aligned;
		}
		else
		{
			ofLogWarning() << "Align FAILED";
		}
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

	float getDepthWidth() const
	{
		return depth_width;
	}

	float getDepthHeight() const
	{
		return depth_height;
	}

	float getColorWidth() const
	{
		if (bAligned)
		{
			if (_color)
			{
				return _color.as<rs2::video_frame>().get_width();
			}
			else
			{
				ofLogWarning() << "color frame is not updated";
				return color_width;
			}
		}
		return color_width;
	}

	float getColorHeight() const
	{
		if (bAligned)
		{
			if (_color)
			{
				return _color.as<rs2::video_frame>().get_height();
			}
			else
			{
				ofLogWarning() << "color frame is not updated";
				return color_height;
			}
		}
		return color_height;
	}

	bool isFrameNew()
	{
		return bFrameNew;
	}

	shared_ptr<ofTexture> getDepthTexture() const
	{
		return depth_texture;
	}

	shared_ptr<ofTexture> getDepthRawTexture() const
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

	void setDepthColorSchema(COLOR_SCHEMA schema)
	{
		rs2colorizer.set_option(RS2_OPTION_COLOR_SCHEME, schema);
	}

	rs2_intrinsics getCameraIntrinsics() const
	{
		return intr;
	}

	string getRecordedFilePath()
	{
		return _recordFilePath;
	}

private:
	rs2::device		rs2device;
	Rs2Config		rs2config; 
	rs2::colorizer  rs2colorizer;
	shared_ptr<rs2::pipeline>	rs2_pipeline;

	rs2::frame_queue rs2depth_queue;
	rs2::frame_queue rs2video_queue;
	rs2::frame_queue rs2ir_queue;

	rs2::frame _depth;
	rs2::frame _color;
	rs2::frame _ir;
	rs2_intrinsics intr;

	bool _isRecording;
	int _color_fps, _ir_fps, _depth_fps;
	vector<shared_ptr<ofxlibrealsense2p::Filter>> filters;
	float depthScale;
	int deviceId;
	bool color_enabled = false, ir_enabled = false, depth_enabled = false;
	atomic_bool _isFrameNew;
	bool bFrameNew = false;
	int color_width, color_height;
	int ir_width, ir_height;
	int depth_width, depth_height;
	int original_depth_width, original_depth_height;
	bool bReadFile = false;
	bool bAligned = false;
	bool bUseArbTexDepth = true;
	string readFilePath;
	string _recordFilePath;
	ofPtr<ofTexture> depth_texture, raw_depth_texture, color_texture, ir_tex;
	rs2::disparity_transform* disparity_to_depth;
	void listSensorProfile();
	void listStreamingProfile(const rs2::sensor& sensor);
	void process();
	bool _useThread = true;
	double _seekPosition = -1;

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

	void allocateDepthBuffer(float width, float height)
	{
		if (!depth_texture->isAllocated() || (depth_texture->getWidth() != width || depth_texture->getHeight() != height))
		{
			if(depth_texture->isAllocated())depth_texture->clear();
			depth_texture->allocate(width, height, GL_RGB, bUseArbTexDepth);
		}
		if (!raw_depth_texture->isAllocated() || (raw_depth_texture->getWidth() != width || raw_depth_texture->getHeight() != height))
		{
			raw_depth_texture->clear();
			raw_depth_texture->allocate(width, height, GL_R16, bUseArbTexDepth);
		}
		depth_width = width;
		depth_height = height;
		raw_depth_texture->setRGToRGBASwizzles(true);
	}
};
