#pragma once

#include <librealsense2/rs.hpp>
#include <librealsense2/rsutil.h>
#include "ofMain.h"
#include "ofxLRS2/Filter.h"
#include "ofxLRS2/Rs2Config.h"
#include "ofxLRS2/BaseFrameUnit.h"

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
		if (_depth_frame && _color_frame)
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
		if (_raw_depth_frame)
			_raw_depth_frame->drawTexture(x, y);
	}
	
	void drawDepth(int x, int y)
	{
		if(_depth_frame)
			_depth_frame->drawTexture(x, y);
	}
	
	void drawColor(int x, int y)
	{
		if (_color_frame)
			_color_frame->drawTexture(x, y);
	}
	
	void drawIR(int x, int y)
	{
		if (_ir_frame)
			_ir_frame->drawTexture(x, y);
	}
	
	float getDepthWidth() const
	{
		return _depth_frame->getWidth();
	}
	
	float getDepthHeight() const
	{
		return _depth_frame->getHeight();
	}
	
	float getColorWidth() const
	{
		return _color_frame->getWidth();//if it is aligned, then it would not the same value as its originals
	}
	
	float getColorHeight() const
	{
		return _color_frame->getHeight();//if it is aligned, then it would not the same value as its originals
	}
	
	bool isFrameNew()
	{
		return bFrameNew;
	}
	
	shared_ptr<ofTexture> getDepthTexture() const
	{
		return _depth_frame->getTexture();
	}
	
	shared_ptr<ofTexture> getDepthRawTexture() const
	{
		return _raw_depth_frame->getTexture();
	}
	
	shared_ptr<ofTexture> getColorTexture()
	{
		return _color_frame->getTexture();
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
	
	const std::vector<glm::vec3>& getPointCloud() {
		static std::vector<glm::vec3> points;
		
		if (!_depth) return points;
		
		// Generate point cloud
		_points = _pc.calculate(_depth);
		
		// Get vertices
		auto vertices = _points.get_vertices();
		auto size = _points.size();
		
		// Resize vector if needed
		points.resize(size);
		
		// Copy points - vertices is a contiguous array of xyz floats
		for (size_t i = 0; i < size; i++) {
			const auto& v = vertices[i];
			points[i] = glm::vec3(v.x, v.y, v.z);
		}
		
		return points;
	}
	
	private:
	rs2::device		rs2device;
	Rs2Config		rs2config;
	rs2::colorizer  rs2colorizer;
	shared_ptr<rs2::pipeline>	rs2_pipeline;
	
	rs2::frame_queue rs2depth_queue;
	//rs2::frame_queue rs2video_queue;
	//rs2::frame_queue rs2ir_queue;
	
	rs2::frame _depth;
	rs2_intrinsics intr;
	
	shared_ptr<BaseFrameUnit<>> _color_frame, _ir_frame, _depth_frame;
	shared_ptr<BaseFrameUnit<unsigned short>> _raw_depth_frame;
	
	bool _isRecording;
	vector<shared_ptr<ofxlibrealsense2p::Filter>> filters;
	float depthScale;
	int deviceId;
	atomic_bool _isFrameNew;
	bool bFrameNew = false;
	int original_depth_width, original_depth_height;
	bool bReadFile = false;
	bool bAligned = false;
	bool bUseArbTexDepth = true;
	string readFilePath;
	string _recordFilePath;
	rs2::disparity_transform* disparity_to_depth;
	void listSensorProfile();
	void listStreamingProfile(const rs2::sensor& sensor);
	void process();
	bool _useThread = true;
	double _seekingPosition = -1;
	
	rs2::pointcloud _pc;
	rs2::points _points;
	
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
	
	void updateDepthFrameForPostProcess(rs2::depth_frame& depthFrame)
	{
		bool revert_disparity = false;
		for (auto f : filters)
		{
			if (*(f->getIsEnabled()))
			{
				depthFrame = f->getFilter()->process(depthFrame);
				if (f->getFilterName() == "Disparity")
				{
					revert_disparity = true;
				}
			}
		}
		if (revert_disparity)
		{
			depthFrame = disparity_to_depth->process(depthFrame);
		}
	}
};
