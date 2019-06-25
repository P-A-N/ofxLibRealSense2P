#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	realsense.load("record.bag");
	realsense.enableDepth(640, 480);
	realsense.enableColor(640, 480);
	realsense.startStream();
}

//--------------------------------------------------------------
void ofApp::update(){
	realsense.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	realsense.drawColor(0,0);
	realsense.drawDepth(realsense.getColorWidth(), 0);

	float x = ofMap(realsense.getPosition(), 0.0, 1.0, 0.0, ofGetWidth());
	ofPushStyle();
	ofSetColor(255, 0, 0);
	ofDrawLine(x, 0, x, ofGetHeight());
	ofPopStyle();

	ofDrawBitmapStringHighlight("duration nano sec : " + std::to_string(realsense.getDurationAs<ofxLibRealSense2P::nano_sec>()), 10, 10);
	ofDrawBitmapStringHighlight("duration mill sec : " + std::to_string(realsense.getDurationAs<ofxLibRealSense2P::mill_sec>()), 10, 30);
	ofDrawBitmapStringHighlight("duration sec : " + std::to_string(realsense.getDurationAs<ofxLibRealSense2P::sec>()), 10, 50);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
