#include "ofxLibRealSense2P.h"


void ofxLibRealSense2P::setupDevice(int deviceID)
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
	cout << "Device name is: " << rs2device.get_info(RS2_CAMERA_INFO_NAME) << endl;
	this->deviceId = deviceID;
}

void ofxLibRealSense2P::load(string path)
{
	ofFile file(path);
	cout << "load file:" << file.getAbsolutePath() << endl;
	rs2config.enable_device_from_file(file.getAbsolutePath());
	bReadFile = true;
}

void ofxLibRealSense2P::startStream()
{
	rs2::pipeline_profile profile = rs2_pipeline.start(rs2config);
	depthScale = calcDepthScale(profile.get_device());
	if (bReadFile)
	{
		rs2device = rs2_pipeline.get_active_profile().get_device();
		string deviceSerial = rs2device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
		rs2config.enable_device(deviceSerial);
		cout << "Device name is: " << rs2device.get_info(RS2_CAMERA_INFO_NAME) << endl;
	}
	start();
}

void ofxLibRealSense2P::enableColor(int width, int height, int fps)
{
	color_width = width;
	color_height = height;
	color_texture = make_shared<ofTexture>();
	color_texture->allocate(color_width, color_height, GL_RGB);
	if(!bReadFile)
		rs2config.enable_stream(RS2_STREAM_COLOR, -1, color_width, color_height, RS2_FORMAT_RGB8, fps);
	color_enabled = true;
}

void ofxLibRealSense2P::enableIr(int width, int height, int fps)
{
	ir_width = width;
	ir_height = height;
	ir_tex = make_shared<ofTexture>();
	ir_tex->allocate(ir_width, ir_height, GL_LUMINANCE);
	if (!bReadFile)
		rs2config.enable_stream(RS2_STREAM_INFRARED, -1, ir_width, ir_height, RS2_FORMAT_Y8, fps);
	ir_enabled = true;
}

void ofxLibRealSense2P::enableDepth(int width, int height, int fps)
{
	depth_width = width;
	depth_height = height;
	depth_texture = make_shared<ofTexture>();
	raw_depth_texture = make_shared<ofTexture>();
	depth_texture->allocate(depth_width, depth_height, GL_RGB);
	raw_depth_texture->allocate(depth_width, depth_height, GL_R16);
	raw_depth_texture->setRGToRGBASwizzles(true);
	if (!bReadFile)
	{
		rs2config.enable_stream(RS2_STREAM_DEPTH, -1, depth_width, depth_height, RS2_FORMAT_Z16, fps);
	}
	rs2colorizer.set_option(RS2_OPTION_COLOR_SCHEME, 2);
	depth_enabled = true;
}

void ofxLibRealSense2P::update()
{
	bFrameNew = _isFrameNew.load(memory_order_acquire);
	if (bFrameNew)
	{
		if (_colBuff.isAllocated())
		{
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
			if (!_depthBuff.isAllocated() || _depthBuff.getWidth() != normalizedDepthFrame.get_width() || _depthBuff.getHeight() != normalizedDepthFrame.get_height())
			{
				_depthBuff.allocate(normalizedDepthFrame.get_width(), normalizedDepthFrame.get_height(), 3);
			}
			memcpy(_depthBuff.getData(),(uint8_t *)normalizedDepthFrame.get_data(), normalizedDepthFrame.get_width() * normalizedDepthFrame.get_height() * sizeof(uint8_t) * 3);
			raw_depth_texture->loadData(_rawDepthBuff);
			depth_texture->loadData(_depthBuff);
		}
	}
	_isFrameNew.store(false, memory_order_relaxed);
}


glm::vec3 ofxLibRealSense2P::getWorldCoordinateAt(float x, float y)
{
	glm::vec3 result;
	if (!_depth || !(x >= 0 && x < getDepthWidth() && y >= 0 && y < getDepthHeight())) return result;
	rs2_intrinsics intr = _depth.get_profile().as<rs2::video_stream_profile>().get_intrinsics();
	float distance= _depth.as<rs2::depth_frame>().get_distance(x, y);
	float _in[2] = { x,y };
	rs2_deproject_pixel_to_point(glm::value_ptr(result), &intr, _in, distance);
	return result;
}

float ofxLibRealSense2P::getDistanceAt(int x, int y)
{
	if (!_depth) return 0;
	return _depth.as<rs2::depth_frame>().get_distance(x, y);
}

void ofxLibRealSense2P::threadedFunction()
{
	rs2::frameset frame, alignedFrame;
	while (isThreadRunning())
	{
		if (rs2_pipeline.poll_for_frames(&frame)) {
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
				if (!_rawDepthBuff.isAllocated() || _rawDepthBuff.getWidth() != depthFrame.get_width() || _rawDepthBuff.getHeight() != depthFrame.get_height())
				{
					_rawDepthBuff.allocate(depthFrame.get_width(), depthFrame.get_height(), 1);
				}
				memcpy(_rawDepthBuff.getData(), (uint16_t*)depthFrame.get_data(), depthFrame.get_width() * depthFrame.get_height() * sizeof(uint16_t));
				//ofLog() << (unsigned short)_rawDepthBuff.getData()[0] << " " << ((uint16_t *)depthFrame.get_data())[0];
				//rs2::video_frame normalizedDepthFrame = rs2colorizer.process(depthFrame);
				//_depthBuff = (uint8_t *)normalizedDepthFrame.get_data();
				rs2depth_queue.enqueue(depthFrame);
				
			}
			_isFrameNew.store(true, memory_order_release);
		}
	}
}

ofxGuiGroup* ofxLibRealSense2P::setupGUI()
{
	rs2::sensor sensor = rs2device.query_sensors()[this->deviceId];
	rs2::option_range orExp = sensor.get_option_range(RS2_OPTION_EXPOSURE);
	rs2::option_range orGain = sensor.get_option_range(RS2_OPTION_GAIN);
	rs2::option_range orMinDist = rs2colorizer.get_option_range(RS2_OPTION_MIN_DISTANCE);
	rs2::option_range orMaxDist = rs2colorizer.get_option_range(RS2_OPTION_MAX_DISTANCE);

	_D400Params.setup("D400_" + device_serial);
	_D400Params.add(_autoExposure.setup("Auto exposure", true));
	_D400Params.add(_enableEmitter.setup("Emitter", true));
	_D400Params.add(_irExposure.setup("IR Exposure", orExp.def, orExp.min, 26000));
	_D400Params.add(_depthMin.setup("Min Depth", orMinDist.def, orMinDist.min, orMinDist.max));
	_D400Params.add(_depthMax.setup("Max Depth", orMaxDist.def, orMaxDist.min, orMaxDist.max));

	_autoExposure.addListener(this, &ofxLibRealSense2P::onD400BoolParamChanged);
	_enableEmitter.addListener(this, &ofxLibRealSense2P::onD400BoolParamChanged);
	_irExposure.addListener(this, &ofxLibRealSense2P::onD400IntParamChanged);
	_depthMin.addListener(this, &ofxLibRealSense2P::onD400ColorizerParamChanged);
	_depthMax.addListener(this, &ofxLibRealSense2P::onD400ColorizerParamChanged);

	return &_D400Params;
}

void ofxLibRealSense2P::onD400BoolParamChanged(bool &value)
{
	rs2::sensor sensor = rs2_pipeline.get_active_profile().get_device().first<rs2::depth_sensor>();
	if (sensor.supports(RS2_OPTION_ENABLE_AUTO_EXPOSURE))
		sensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, _autoExposure ? 1.0f : 0.0f);
	if (sensor.supports(RS2_OPTION_EMITTER_ENABLED))
		sensor.set_option(RS2_OPTION_EMITTER_ENABLED, _enableEmitter ? 1.0f : 0.0f);
}


void ofxLibRealSense2P::onD400IntParamChanged(int &value)
{
	rs2::sensor sensor = rs2_pipeline.get_active_profile().get_device().first<rs2::depth_sensor>();
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