#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	realsense.setupDevice();
	realsense.enableDepth(640, 480, 30);
	realsense.startStream();
}

//--------------------------------------------------------------
void ofApp::update(){
	realsense.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	realsense.drawDepth(0, 0);
	ofDrawBitmapString("key up or down for change color schema : current schema id is " + ofToString(colorSchemaId), 30, ofGetHeight() - 50);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == OF_KEY_DOWN)
	{
		colorSchemaId++;
		if (colorSchemaId >= TOTAL_SCHEMA_NUM)
		{
			colorSchemaId = 0;
		}
	}
	else if (key == OF_KEY_UP)
	{
		colorSchemaId--;
		if (colorSchemaId < 0)
		{
			colorSchemaId = TOTAL_SCHEMA_NUM-1;
		}
	}
	realsense.setDepthColorSchema((ofxLibRealSense2P::COLOR_SCHEMA)colorSchemaId);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
