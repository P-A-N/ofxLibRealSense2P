#pragma once
#include "ofxLibRealSense2P.h"
#include <librealsense2/rs.hpp> 
#include <librealsense2/rsutil.h>

class GuiUtils
{
public:
	GuiUtils();
	~GuiUtils();

	ofParameterGroup& setupGUI(ofxLibRealSense2P* realsense);
	ofParameterGroup& getGui();
private:

	void onD400BoolParamChanged(bool &value);
	void onD400IntParamChanged(int &value);
	void onD400ColorizerParamChanged(float &value);
	//attribute
	ofParameterGroup     _D400Params;
	ofParameter<bool>       _autoExposure;
	ofParameter<bool>       _enableEmitter;
	ofParameter<int>    _irExposure;
	ofParameter<float>  _depthMin;
	ofParameter<float>  _depthMax;
	ofxLibRealSense2P* realsense;
};

