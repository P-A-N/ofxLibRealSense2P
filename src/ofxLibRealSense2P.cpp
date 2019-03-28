#include "ofxLibRealSense2P.h"


void ofxLibRealSense2P::setupDevice(int deviceID, bool listAvailableStream)
{
	rs2::context ctx;
	rs2::device_list devList = ctx.query_devices();
	if (devList.size() <= 0)
	{
		ofSystemAlertDialog("RealSense device not found!");
		return;
	}

	if (deviceID >= devList.size()) {
		ofSystemAlertDialog("Requested device id is invalid");
		return;
	}
	rs2device = devList[deviceID];
	device_serial = rs2device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
	rs2config.enable_device(device_serial);
	this->deviceId = deviceID;
	if (listAvailableStream)
	{
		cout << "Device name is: " << rs2device.get_info(RS2_CAMERA_INFO_NAME) << endl;
		listSensorProfile();
	}
	setupFilter();
	_isRecording = false;
}

void ofxLibRealSense2P::load(string path)
{
	ofFile file(path);
	cout << "load file:" << file.getAbsolutePath() << endl;
	rs2config.enable_device_from_file(file.getAbsolutePath());
	setupFilter();
	bReadFile = true;
}

void ofxLibRealSense2P::setupFilter()
{
	filters.emplace_back(make_shared<ofxlibrealsense2p::Filter>(ofxlibrealsense2p::Filter::DECIMATION));
	filters.emplace_back(make_shared<ofxlibrealsense2p::Filter>(ofxlibrealsense2p::Filter::THRESHOLD));
	filters.emplace_back(make_shared<ofxlibrealsense2p::Filter>(ofxlibrealsense2p::Filter::DISPARITY));
	filters.emplace_back(make_shared<ofxlibrealsense2p::Filter>(ofxlibrealsense2p::Filter::SPATIAL));
	filters.emplace_back(make_shared<ofxlibrealsense2p::Filter>(ofxlibrealsense2p::Filter::TEMPORAL));
	disparity_to_depth = new rs2::disparity_transform(false);
}

void ofxLibRealSense2P::startStream()
{
	rs2_pipeline = make_shared<rs2::pipeline>();
	rs2::pipeline_profile profile = rs2_pipeline->start(rs2config);
	depthScale = calcDepthScale(profile.get_device());
	if (bReadFile)
	{
		rs2device = rs2_pipeline->get_active_profile().get_device();
		string deviceSerial = rs2device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
		rs2config.enable_device(deviceSerial);
		cout << "Device name is: " << rs2device.get_info(RS2_CAMERA_INFO_NAME) << endl;
	}
	start();
}

void ofxLibRealSense2P::enableColor(int width, int height, int fps, bool useArbTex)
{
	color_width = width;
	color_height = height;
	color_texture = make_shared<ofTexture>();
	color_texture->allocate(color_width, color_height, GL_RGB, useArbTex);
	_color_fps = fps;
	if(!bReadFile)
		rs2config.enable_stream(RS2_STREAM_COLOR, -1, color_width, color_height, RS2_FORMAT_RGB8, fps);
	color_enabled = true;
}

void ofxLibRealSense2P::enableIr(int width, int height, int fps, bool useArbTex)
{
	ir_width = width;
	ir_height = height;
	_ir_fps = fps;
	ir_tex = make_shared<ofTexture>();
	ir_tex->allocate(ir_width, ir_height, GL_LUMINANCE, useArbTex);
	if (!bReadFile)
		rs2config.enable_stream(RS2_STREAM_INFRARED, -1, ir_width, ir_height, RS2_FORMAT_Y8, fps);
	ir_enabled = true;
}

void ofxLibRealSense2P::enableDepth(int width, int height, int fps, bool useArbTex)
{
	bUseArbTexDepth = useArbTex;
	original_depth_width = depth_width = width;
	original_depth_height = depth_height = height;
	_depth_fps = fps;
	depth_texture = make_shared<ofTexture>();
	raw_depth_texture = make_shared<ofTexture>();
	allocateDepthBuffer(width, height);
	if (!bReadFile)
	{
		rs2config.enable_stream(RS2_STREAM_DEPTH, -1, depth_width, depth_height, RS2_FORMAT_Z16, fps);
		//intr = _depth.get_profile().as<rs2::video_stream_profile>().get_intrinsics();
	}
	rs2colorizer.set_option(RS2_OPTION_COLOR_SCHEME, COLOR_SCHEMA_WhiteToBlack);
	depth_enabled = true;
}

void ofxLibRealSense2P::startRecord(string path)
{
	if (!rs2device.as<rs2::playback>())
	{
		if (!rs2device.as<rs2::recorder>())
		{
			rs2_pipeline->stop();
			waitForThread(true);
			cout << "wait thread stop";
			while (isThreadRunning())
			{
				cout << ".";
			}
			cout << endl;
			rs2_pipeline = make_shared< rs2::pipeline>();
			rs2::config cfg = rs2config;
			if(color_enabled)
				cfg.enable_stream(RS2_STREAM_COLOR, -1, color_width, color_height, RS2_FORMAT_RGB8, _color_fps);
			if(ir_enabled)
				cfg.enable_stream(RS2_STREAM_INFRARED, -1, ir_width, ir_height, RS2_FORMAT_Y8, _ir_fps);
			if(depth_enabled)
				cfg.enable_stream(RS2_STREAM_DEPTH, -1, original_depth_width, original_depth_height, RS2_FORMAT_Z16, _depth_fps);
		
			ofDirectory::createDirectory("data", false);
			cfg.enable_record_to_file("data/"+path);
			rs2_pipeline->start(cfg);
			rs2device = rs2_pipeline->get_active_profile().get_device();
			_isRecording = true;
			_recordFilePath = path;
			startThread();
		}
	}
}

void ofxLibRealSense2P::stopRecord(bool bPlayback)
{
	if (rs2device.as<rs2::recorder>())
	{
		waitForThread(true);
		rs2_pipeline->wait_for_frames();
		rs2_pipeline->stop();
	}
	//if (bPlayback)
	//{
	//	cout << "playback " << _recordFilePath << endl;
	//	playbackRecorded();
	//}
	//else
	{
		rs2_pipeline = make_shared< rs2::pipeline>();
		rs2::config cfg;
		if (color_enabled)
			cfg.enable_stream(RS2_STREAM_COLOR, -1, color_width, color_height, RS2_FORMAT_RGB8, _color_fps);
		if (ir_enabled)
			cfg.enable_stream(RS2_STREAM_INFRARED, -1, ir_width, ir_height, RS2_FORMAT_Y8, _ir_fps);
		if (depth_enabled)
			cfg.enable_stream(RS2_STREAM_DEPTH, -1, original_depth_width, original_depth_height, RS2_FORMAT_Z16, _depth_fps);
		rs2::pipeline_profile profile = rs2_pipeline->start(cfg);
		rs2device = rs2_pipeline->get_active_profile().get_device();
		depthScale = calcDepthScale(profile.get_device());
		_isRecording = false;
		startThread();
	}
}

//not working!!!! 
void ofxLibRealSense2P::playbackRecorded()
{
	if (!rs2device.as<rs2::playback>())
	{
		if (isThreadRunning())
		{
			rs2_pipeline->stop();
			waitForThread(true);
		}
		rs2::config cfg = rs2config;
		cfg.enable_device_from_file("data/"+_recordFilePath);
		rs2_pipeline->start(cfg); //File will be opened in read mode at this point
		rs2device = rs2_pipeline->get_active_profile().get_device();
	}
}

bool ofxLibRealSense2P::isRecording()
{
	return _isRecording;
}

void ofxLibRealSense2P::update()
{
	if (!_useThread)process();
	bFrameNew = _isFrameNew.load(memory_order_acquire);
	if (bFrameNew)
	{
		if (_colBuff.isAllocated())
		{
			rs2video_queue.poll_for_frame(&_color);
			color_texture->loadData(_colBuff.getData(), color_width, color_height, GL_RGB);
		}
		if (_irBuff.isAllocated())
		{
			ir_tex->loadData(_irBuff.getData(), ir_width, ir_height, GL_LUMINANCE);
		}
		if (_rawDepthBuff.isAllocated())
		{
			rs2depth_queue.poll_for_frame(&_depth);
			rs2::video_frame normalizedDepthFrame = rs2colorizer.process(_depth.as<rs2::depth_frame>());
			if (!raw_depth_texture->isAllocated() || raw_depth_texture->getWidth() != normalizedDepthFrame.get_width() || raw_depth_texture->getHeight() != normalizedDepthFrame.get_height())
			{
				//width - height will be changed when Decimate filter applied. reallocate when the size is different
				allocateDepthBuffer(normalizedDepthFrame.get_width(), normalizedDepthFrame.get_height());
			}
			if (_depth && intr.width != normalizedDepthFrame.get_width() || intr.height != normalizedDepthFrame.get_height())
			{
				intr = _depth.get_profile().as<rs2::video_stream_profile>().get_intrinsics();
			}
			memcpy(_depthBuff.getData(),(uint8_t *)normalizedDepthFrame.get_data(), normalizedDepthFrame.get_width() * normalizedDepthFrame.get_height() * sizeof(uint8_t) * 3);
			raw_depth_texture->loadData(_rawDepthBuff);
			depth_texture->loadData(_depthBuff.getData(), _depthBuff.getWidth(), _depthBuff.getHeight(), GL_RGB);
		}
	}
	_isFrameNew.store(false, memory_order_relaxed);
}

void ofxLibRealSense2P::threadedFunction()
{
	while(isThreadRunning() && _useThread)
	{
		process();
	}
	cout << "thread run end" << endl;
}

void ofxLibRealSense2P::process()
{
	rs2::frameset frame, alignedFrame;
	if (rs2_pipeline->poll_for_frames(&frame)) {
		if (bAligned)
		{
			rs2::align align = rs2::align(RS2_STREAM_COLOR);
			frame = align.process(frame);
		}
		if (color_enabled) {
			rs2::video_frame colFrame = frame.get_color_frame();
			if (!_colBuff.isAllocated() || _colBuff.getWidth() != colFrame.get_width() || _colBuff.getHeight() != colFrame.get_height())
			{
				_colBuff.allocate(colFrame.get_width(), colFrame.get_height(), 3);
			}
			memcpy(_colBuff.getData(), (uint8_t*)colFrame.get_data(), colFrame.get_width() * colFrame.get_height() * sizeof(uint8_t) * 3);
			rs2video_queue.enqueue(colFrame);
		}
		if (ir_enabled) {
			rs2::video_frame irFrame = frame.get_infrared_frame();
			if (!_irBuff.isAllocated() || _irBuff.getWidth() != irFrame.get_width() || _irBuff.getHeight() != irFrame.get_height())
			{
				_irBuff.allocate(irFrame.get_width(), irFrame.get_height(), 3);
			}
			memcpy(_irBuff.getData(), (uint8_t*)irFrame.get_data(), irFrame.get_width() * irFrame.get_height() * sizeof(uint8_t));
			rs2ir_queue.enqueue(irFrame);
		}
		if (depth_enabled) {
			rs2::depth_frame depthFrame = frame.get_depth_frame();
			//ofLog() << (unsigned short)_rawDepthBuff.getData()[0] << " " << ((uint16_t *)depthFrame.get_data())[0];
			//rs2::video_frame normalizedDepthFrame = rs2colorizer.process(depthFrame);
			//_depthBuff = (uint8_t *)normalizedDepthFrame.get_data();
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
			if (!_rawDepthBuff.isAllocated() || _rawDepthBuff.getWidth() != depthFrame.get_width() || _rawDepthBuff.getHeight() != depthFrame.get_height())
			{
				_rawDepthBuff.allocate(depthFrame.get_width(), depthFrame.get_height(), 1);
			}
			memcpy(_rawDepthBuff.getData(), (uint16_t*)depthFrame.get_data(), depthFrame.get_width() * depthFrame.get_height() * sizeof(uint16_t));
			if (depth_width != depthFrame.get_width()) depth_width = depthFrame.get_width(); //width will be changed when Decimate filter applied
			if (depth_height != depthFrame.get_width()) depth_height = depthFrame.get_height(); //width will be changed when Decimate filter applied

			rs2depth_queue.enqueue(depthFrame);

		}
		_isFrameNew.store(true, memory_order_release);
	}
}

glm::vec3 ofxLibRealSense2P::getWorldCoordinateAt(float x, float y)
{
	glm::vec3 result;
	if (!_depth || !(x >= 0 && x < getDepthWidth() && y >= 0 && y < getDepthHeight())) return result;
	float distance = _depth.as<rs2::depth_frame>().get_distance(x, y);
	float _in[2] = { x,y };
	rs2_deproject_pixel_to_point(glm::value_ptr(result), &intr, _in, distance);
	return result;
}

float ofxLibRealSense2P::getDistanceAt(int x, int y)
{
	if (!_depth) return 0;
	return _depth.as<rs2::depth_frame>().get_distance(x, y);
}

ofxGuiGroup* ofxLibRealSense2P::setupGUI()
{
	rs2::sensor sensor = rs2device.query_sensors()[this->deviceId];
	rs2::option_range orExp = sensor.get_option_range(RS2_OPTION_EXPOSURE);
	rs2::option_range orGain = sensor.get_option_range(RS2_OPTION_GAIN);
	//rs2::option_range orMinDist = rs2colorizer.get_option_range(RS2_OPTION_MIN_DISTANCE);
	//rs2::option_range orMaxDist = rs2colorizer.get_option_range(RS2_OPTION_MAX_DISTANCE);

	_D400Params.setup("D400_" + device_serial);
	_D400Params.add(_autoExposure.setup("Auto exposure", true));
	_D400Params.add(_enableEmitter.setup("Emitter", true));
	_D400Params.add(_irExposure.setup("IR Exposure", orExp.def, orExp.min, 26000));
	//_D400Params.add(_depthMin.setup("Min Depth", orMinDist.def, orMinDist.min, orMinDist.max));
	//_D400Params.add(_depthMax.setup("Max Depth", orMaxDist.def, orMaxDist.min, orMaxDist.max));

	for (auto filter : filters)
	{
		ofParameterGroup group;
		group.setName(filter->getFilterName());
		group.add(*filter->getIsEnabled());
		for (auto s : filter->getSliders())
		{
			group.add(*s);
		}
		_D400Params.add(group);
	}

	_autoExposure.addListener(this, &ofxLibRealSense2P::onD400BoolParamChanged);
	_enableEmitter.addListener(this, &ofxLibRealSense2P::onD400BoolParamChanged);
	_irExposure.addListener(this, &ofxLibRealSense2P::onD400IntParamChanged);
	//_depthMin.addListener(this, &ofxLibRealSense2P::onD400ColorizerParamChanged);
	//_depthMax.addListener(this, &ofxLibRealSense2P::onD400ColorizerParamChanged);

	return &_D400Params;
}

void ofxLibRealSense2P::onD400BoolParamChanged(bool &value)
{
	rs2::sensor sensor = rs2_pipeline->get_active_profile().get_device().first<rs2::depth_sensor>();
	if (sensor.supports(RS2_OPTION_ENABLE_AUTO_EXPOSURE))
		sensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, _autoExposure ? 1.0f : 0.0f);
	if (sensor.supports(RS2_OPTION_EMITTER_ENABLED))
		sensor.set_option(RS2_OPTION_EMITTER_ENABLED, _enableEmitter ? 1.0f : 0.0f);
}


void ofxLibRealSense2P::onD400IntParamChanged(int &value)
{
	rs2::sensor sensor = rs2_pipeline->get_active_profile().get_device().first<rs2::depth_sensor>();
	if (sensor.supports(RS2_OPTION_EXPOSURE))
		sensor.set_option(RS2_OPTION_EXPOSURE, (float)_irExposure);
}


void ofxLibRealSense2P::onD400ColorizerParamChanged(float &value)
{
	rs2colorizer.set_option(RS2_OPTION_HISTOGRAM_EQUALIZATION_ENABLED, 0);

	if (rs2colorizer.supports(RS2_OPTION_MIN_DISTANCE))
		rs2colorizer.set_option(RS2_OPTION_MIN_DISTANCE, _depthMin);
	if (rs2colorizer.supports(RS2_OPTION_MAX_DISTANCE))
		rs2colorizer.set_option(RS2_OPTION_MAX_DISTANCE, _depthMax);
}


ofxGuiGroup* ofxLibRealSense2P::getGui()
{
	return &_D400Params;
}

void ofxLibRealSense2P::listSensorProfile()
{
	vector<rs2::sensor> sensors = rs2device.query_sensors();
	std::cout << "Device consists of " << sensors.size() << " sensors:\n" << std::endl;
	int index = 0;
	for (rs2::sensor sensor : sensors)
	{
		if (sensor.supports(RS2_CAMERA_INFO_NAME))
		{
			std::cout << "  " << index++ << " : " << sensor.get_info(RS2_CAMERA_INFO_NAME) << std::endl;
			listStreamingProfile(sensor);
		}
		else
		{
			std::cout << "  " << index++ << " : Unknown Sensor" << endl;
			listStreamingProfile(sensor);
		}

	}
}

void ofxLibRealSense2P::listStreamingProfile(const rs2::sensor& sensor)
{
	std::cout << "  Sensor supports the following profiles:\n" << std::endl;
	std::vector<rs2::stream_profile> stream_profiles = sensor.get_stream_profiles();
	int profile_num = 0;
	for (rs2::stream_profile stream_profile : stream_profiles)
	{
		rs2_stream stream_data_type = stream_profile.stream_type();
		int stream_index = stream_profile.stream_index();
		std::string stream_name = stream_profile.stream_name();            
		int unique_stream_id = stream_profile.unique_id(); // The unique identifier can be used for comparing two streams
		std::cout << std::setw(5) << profile_num << ": " << stream_data_type << " #" << stream_index;
		if (stream_profile.is<rs2::video_stream_profile>()) //"Is" will test if the type tested is of the type given
		{
			// "As" will try to convert the instance to the given type
			rs2::video_stream_profile video_stream_profile = stream_profile.as<rs2::video_stream_profile>();

			// After using the "as" method we can use the new data type
			//  for additinal operations:
			std::cout << " (Video Stream: " << video_stream_profile.format() << " " <<
				video_stream_profile.width() << "x" << video_stream_profile.height() << "@ " << video_stream_profile.fps() << "Hz)";
		}
		std::cout << std::endl;
		profile_num++;
	}
	std::cout << std::endl;

}