#pragma once
#include <librealsense2/rs.hpp> 
#include "ofxGui.h"

namespace ofxLibRealsense2P
{
	class FilterOption
	{
	public:
		FilterOption() {}
		~FilterOption() {}
	private:
		void setup(std::shared_ptr<rs2::filter> filter, rs2_option& option, float value, float min, float max, bool isInt);
		void onD400FilterFloatParamChanged(float &value);
		void onD400FilterIntParamChanged(int &value);
		std::shared_ptr<rs2::filter> _filter;
		rs2_option _option;
		std::shared_ptr<ofAbstractParameter> _slider;
		friend class Filter;
	};


	class Filter
	{
	public:
		Filter();
		~Filter();
		// Decimation - reduces depth frame density
		// Threshold  - removes values outside recommended range
		// Spatial    - edge-preserving spatial smoothing
		// Temporal   - reduces temporal noise
		// disparity  - transform from depth to disparity and vice versa
		enum FILTER_TYPE
		{
			DECIMATION, THRESHOLD, SPATIAL, TEMPORAL, DISPARITY
		};

		Filter(FILTER_TYPE type);

		std::string getFilterName() { return _filter_name; }

		std::vector<std::shared_ptr<ofAbstractParameter>> getSliders()
		{
			std::vector<std::shared_ptr<ofAbstractParameter>> result;
			for (auto o : _options)
			{
				result.emplace_back(o->_slider);
			}
			return result;
		}

		std::shared_ptr<ofParameter<bool>> getIsEnabled()
		{
			return _is_enabled;
		}

		bool is_all_integers(const rs2::option_range& range)
		{
			const auto is_integer = [](float f)
			{
				return (fabs(fmod(f, 1)) < std::numeric_limits<float>::min());
			};

			return is_integer(range.min) && is_integer(range.max) &&
				is_integer(range.def) && is_integer(range.step);
		}


		std::shared_ptr<rs2::filter> getFilter()
		{
			return _filter;
		}

	private:
		FILTER_TYPE _type;
		std::shared_ptr<ofParameter<bool>> _is_enabled;
		std::string _filter_name;
		std::shared_ptr<rs2::filter> _filter;
		std::vector<std::shared_ptr<FilterOption>> _options;

	};
}


