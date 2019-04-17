#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	realsense.setupDevice();
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
	ofBackground(0);
	ofPushMatrix();
	realsense.drawDepth(0,0);
	realsense.drawColor(realsense.getDepthWidth(), 0);
	ofPopMatrix();
	ofDrawBitmapStringHighlight("fps:" + ofToString(ofGetFrameRate(), 2), ofGetWidth() - 160, ofGetHeight() - 20);
	ofDrawBitmapString("press 's' to record and saved when the key released", 40, ofGetHeight() - 40);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 's')
	{
		realsense.startRecord("record.bag");
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == 's')
	{
		realsense.stopRecord();
	}
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

