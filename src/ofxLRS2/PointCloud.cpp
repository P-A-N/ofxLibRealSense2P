#include "ofxLRS2/PointCloud.h"
ofxlibrealsense2p::PointCloud::PointCloud()
{
}


ofxlibrealsense2p::PointCloud::~PointCloud()
{
}


void ofxlibrealsense2p::PointCloud::setup(std::shared_ptr<ofTexture> depth_texture)
{
	_depth_texture = depth_texture;
	_shader.load("shader/shader");
	plane = ofPlanePrimitive(_depth_texture->getWidth(), _depth_texture->getHeight(),
		_depth_texture->getWidth(), _depth_texture->getHeight());
	recv.setup(10000);

}


void ofxlibrealsense2p::PointCloud::update()
{
	if (recv.hasWaitingMessages())
	{
		ofxOscMessage m;
		recv.getNextMessage(m);
		if (m.getAddress() == "/camera")
		{
			cam.setPosition(m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2));
			cam.lookAt(glm::vec3(m.getArgAsFloat(3), m.getArgAsFloat(4), m.getArgAsFloat(5)));
		}
	}
}

void ofxlibrealsense2p::PointCloud::draw(rs2_intrinsics intr)
{
	float diff = plane.getWidth() - _depth_texture->getWidth();
	if (diff > 1 || diff < -1)
	{
		plane = ofPlanePrimitive(_depth_texture->getWidth(), _depth_texture->getHeight(),
				_depth_texture->getWidth(), _depth_texture->getHeight(), OF_PRIMITIVE_POINTS);
		cam.setNearClip(0.1);
		cam.setFarClip(10);
	}
	


	ofPushStyle();
	ofPoint leftTop = cam.screenToWorld(glm::vec3(0, 0, 0));
	ofPoint rightBottom = cam.screenToWorld(glm::vec3(ofGetWidth(), ofGetHeight(), 0));
	float scaleIn3D = (rightBottom.x - leftTop.x) / (float)plane.getWidth();
	float farclip = cam.getFarClip();
	ofSetColor(ofColor::blue);
	cam.begin();
	ofPushMatrix();
	//glm::vec3 transVal = leftTop + glm::vec3(plane.getWidth(), -plane.getHeight(), 0) * scaleIn3D*0.5;
	//ofTranslate(transVal);
	//ofScale(scaleIn3D, scaleIn3D);

	_shader.begin();
	_shader.setUniformTexture("_depth_tex", *_depth_texture.get(), 0);
	_shader.setUniform2f("pp", glm::vec2(intr.ppx, intr.ppy));
	_shader.setUniform2f("focal", glm::vec2(intr.fx, intr.fy));
	//_shader.setUniform3f("transformval", transVal);
	//_shader.setUniform1f("scaleIn3D", scaleIn3D);
	ofPushMatrix();
	plane.drawVertices();
	ofPopMatrix();
	_shader.end();
	ofPopMatrix();
	cam.end();
	ofPopStyle();
	
}