#pragma once
#include "ofMain.h"
//#include "ofxAutoReloadedShader.h"
#include <librealsense2/rs.hpp> 
#include <librealsense2/rsutil.h>

namespace ofxlibrealsense2p
{
	class PointCloud
	{
	public:
		PointCloud();
		~PointCloud();
		void setup(std::shared_ptr<ofTexture> depth_texture);
		void update();
		void draw(rs2_intrinsics intr);

	private:
		ofPlanePrimitive plane;
		ofShader _shader;
		std::shared_ptr<ofTexture> _depth_texture;
		ofCamera cam;
	};
}

