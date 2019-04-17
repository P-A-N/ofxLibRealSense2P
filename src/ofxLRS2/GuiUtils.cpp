#include "GuiUtils.h"



GuiUtils::GuiUtils()
{
}


GuiUtils::~GuiUtils()
{
}

ofParameterGroup& GuiUtils::setupGUI(ofxLibRealSense2P* realsense)
{
	this->realsense = realsense;
	rs2::sensor sensor = realsense->rs2device.query_sensors()[realsense->deviceId];
	rs2::option_range orExp = sensor.get_option_range(RS2_OPTION_EXPOSURE);
	rs2::option_range orGain = sensor.get_option_range(RS2_OPTION_GAIN);
	rs2::option_range orMinDist = realsense->rs2colorizer.get_option_range(RS2_OPTION_MIN_DISTANCE);
	rs2::option_range orMaxDist = realsense->rs2colorizer.get_option_range(RS2_OPTION_MAX_DISTANCE);

	_D400Params.setName("D400_" + realsense->rs2config.getDeviceSerial());
	_D400Params.add(_autoExposure.set("Auto exposure", (bool)sensor.get_option(RS2_OPTION_EXPOSURE)));
	_D400Params.add(_enableEmitter.set("Emitter", (bool)sensor.get_option(RS2_OPTION_EMITTER_ENABLED)));
	_D400Params.add(_irExposure.set("IR Exposure", orExp.def, orExp.min, 26000));
	_D400Params.add(_depthMin.set("Min Depth", orMinDist.def, orMinDist.min, orMinDist.max));
	_D400Params.add(_depthMax.set("Max Depth", orMaxDist.def, orMaxDist.min, orMaxDist.max));

	for (auto filter : realsense->filters)
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

	_autoExposure.addListener(this, &GuiUtils::onD400BoolParamChanged);
	_enableEmitter.addListener(this, &GuiUtils::onD400BoolParamChanged);
	_irExposure.addListener(this, &GuiUtils::onD400IntParamChanged);
	_depthMin.addListener(this, &GuiUtils::onD400ColorizerParamChanged);
	_depthMax.addListener(this, &GuiUtils::onD400ColorizerParamChanged);

	return _D400Params;
}


void GuiUtils::onD400BoolParamChanged(bool &value)
{
	rs2::sensor sensor = realsense->rs2_pipeline->get_active_profile().get_device().first<rs2::depth_sensor>();
	if (sensor.supports(RS2_OPTION_ENABLE_AUTO_EXPOSURE))
		sensor.set_option(RS2_OPTION_ENABLE_AUTO_EXPOSURE, _autoExposure ? 1.0f : 0.0f);
	if (sensor.supports(RS2_OPTION_EMITTER_ENABLED))
		sensor.set_option(RS2_OPTION_EMITTER_ENABLED, _enableEmitter ? 1.0f : 0.0f);
}


void GuiUtils::onD400IntParamChanged(int &value)
{
	rs2::sensor sensor = realsense->rs2_pipeline->get_active_profile().get_device().first<rs2::depth_sensor>();
	if (sensor.supports(RS2_OPTION_EXPOSURE))
		sensor.set_option(RS2_OPTION_EXPOSURE, (float)_irExposure);
}


void GuiUtils::onD400ColorizerParamChanged(float &value)
{
	realsense->rs2colorizer.set_option(RS2_OPTION_HISTOGRAM_EQUALIZATION_ENABLED, 0);

	if (realsense->rs2colorizer.supports(RS2_OPTION_MIN_DISTANCE))
		realsense->rs2colorizer.set_option(RS2_OPTION_MIN_DISTANCE, _depthMin);
	if (realsense->rs2colorizer.supports(RS2_OPTION_MAX_DISTANCE))
		realsense->rs2colorizer.set_option(RS2_OPTION_MAX_DISTANCE, _depthMax);
}


ofParameterGroup& GuiUtils::getGui()
{
	return _D400Params;
}

