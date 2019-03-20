#include "ofxLRS2/Filter.h"

ofxlibrealsense2p::Filter::Filter()
{
}


ofxlibrealsense2p::Filter::~Filter()
{
}

ofxlibrealsense2p::Filter::Filter(FILTER_TYPE type)
{
	switch (type)
	{
	case Filter::DECIMATION:
		_filter_name = "Decimate";
		_filter = std::make_shared<rs2::decimation_filter>();
		break;
	case Filter::THRESHOLD:
		_filter_name = "Threashold";
		_filter = std::make_shared<rs2::threshold_filter>();
		break;
	case Filter::SPATIAL:
		_filter_name = "Spatial";
		_filter = std::make_shared<rs2::spatial_filter>();
		break;
	case Filter::TEMPORAL:
		_filter_name = "Temporal";
		_filter = std::make_shared<rs2::temporal_filter>();
		break;
	case Filter::DISPARITY:
		_filter_name = "Disparity";
		_filter = std::make_shared<rs2::disparity_transform>(true);
		break;
	default:
		break;
	}

	_is_enabled = std::make_shared<ofParameter<bool>>();
	_is_enabled->set(_filter_name, false);
	if (type != DISPARITY)
	{
		const std::array<rs2_option, 5> possible_filter_options = {
			RS2_OPTION_FILTER_MAGNITUDE,
			RS2_OPTION_FILTER_SMOOTH_ALPHA,
			RS2_OPTION_MIN_DISTANCE,
			RS2_OPTION_MAX_DISTANCE,
			RS2_OPTION_FILTER_SMOOTH_DELTA
		};
		//Go over each filter option and create a slider for it
		for (rs2_option opt : possible_filter_options)
		{
			if (_filter->supports(opt))
			{
				rs2::option_range range = _filter->get_option_range(opt);
				_options.emplace_back(std::make_shared<FilterOption>());
				_options.back()->setup(_filter, opt, range.def, range.min, range.max, is_all_integers(range));
			}
		}
	}
}

void ofxlibrealsense2p::FilterOption::setup(std::shared_ptr<rs2::filter> filter, rs2_option& option, float value, float min, float max, bool isInt)
{
	_filter = filter;
	_option = option;
	if (isInt)
	{
		_slider = std::make_shared<ofParameter<int>>();
		std::static_pointer_cast<ofParameter<int>>(_slider)->set(_filter->get_option_name(_option), value, min, max);
		std::static_pointer_cast<ofParameter<int>>(_slider)->addListener(this, &FilterOption::onD400FilterIntParamChanged);
	}
	else
	{
		_slider = std::make_shared<ofParameter<float>>();
		std::static_pointer_cast<ofParameter<float>>(_slider)->set(_filter->get_option_name(_option), value, min, max);
		std::static_pointer_cast<ofParameter<float>>(_slider)->addListener(this, &FilterOption::onD400FilterFloatParamChanged);
	}
}

void ofxlibrealsense2p::FilterOption::onD400FilterFloatParamChanged(float &value)
{
	_filter->set_option(_option, value);
}


void ofxlibrealsense2p::FilterOption::onD400FilterIntParamChanged(int &value)
{
	_filter->set_option(_option, value);
}


