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

void ofxLibRealSense2P::startStream()
{
	rs2_pipeline.start(rs2config);
	start();
}

void ofxLibRealSense2P::enableColor(int width, int height, int fps)
{
	color_width = width;
	color_height = height;
	col_tex.allocate(color_width, color_height, GL_RGB);
	rs2config.enable_stream(RS2_STREAM_COLOR, -1, color_width, color_height, RS2_FORMAT_RGB8, fps);
	color_enabled = true;
}

void ofxLibRealSense2P::enableIr(int width, int height, int fps)
{
	ir_width = width;
	ir_height = height;
	ir_tex.allocate(ir_width, ir_height, GL_LUMINANCE);
	rs2config.enable_stream(RS2_STREAM_INFRARED, -1, ir_width, ir_height, RS2_FORMAT_Y8, fps);
	ir_enabled = true;
}

void ofxLibRealSense2P::enableDepth(int width, int height, int fps)
{
	depth_width = width;
	depth_height = height;
	depth_texture.allocate(depth_width, depth_height, GL_RGB);
	raw_depth_texture.allocate(depth_width, depth_height, GL_LUMINANCE16);
	rs2config.enable_stream(RS2_STREAM_DEPTH, -1, depth_width, depth_height, RS2_FORMAT_Z16, fps);
	rs2colorizer.set_option(RS2_OPTION_COLOR_SCHEME, 2);
	depth_enabled = true;
}

void ofxLibRealSense2P::update()
{
	bFrameNew = _isFrameNew.load(memory_order_acquire);
	if (bFrameNew)
	{
		if (_colBuff)
		{
			col_tex.loadData(_colBuff, color_width, color_height, GL_RGB);
		}
		if (_irBuff)
		{
			ir_tex.loadData(_irBuff, ir_width, ir_height, GL_LUMINANCE);
		}
		if (_depthBuff)
		{
			raw_depth_texture.loadData(_rawDepthBuff, depth_width, depth_height, GL_LUMINANCE);
			depth_texture.loadData(_depthBuff, depth_width, depth_height, GL_RGB);
		}
	}
	_isFrameNew.store(false, memory_order_relaxed);
}

float ofxLibRealSense2P::getDistanceAt(int x, int y)
{
	rs2::frame depth;
	rs2depth_queue.poll_for_frame(&depth);
	return depth.as<rs2::depth_frame>().get_distance(x, y);
}

void ofxLibRealSense2P::threadedFunction()
{
	rs2::frameset frame;
	while (isThreadRunning())
	{
		if (rs2_pipeline.poll_for_frames(&frame)) {
			if (color_enabled) {
				rs2::video_frame colFrame = frame.get_color_frame();
				_colBuff = (uint8_t*)colFrame.get_data();
				rs2video_queue.enqueue(colFrame);
			}
			if (ir_enabled) {
				rs2::video_frame irFrame = frame.get_infrared_frame();
				_irBuff = (uint8_t*)irFrame.get_data();
				rs2ir_queue.enqueue(irFrame);
			}
			if (depth_enabled) {
				rs2::depth_frame depthFrame = frame.get_depth_frame();
				_rawDepthBuff = (uint16_t *)depthFrame.get_data();
				rs2::video_frame normalizedDepthFrame = rs2colorizer.process(depthFrame);
				_depthBuff = (uint8_t *)normalizedDepthFrame.get_data();
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