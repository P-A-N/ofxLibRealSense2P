#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	realsense.setupDevice();
	realsense.enableDepth(640, 480);
	realsense.enableColor(640, 480);
	realsense.startStream();

	gui.setup();
	gui.add(realsense.setupGUI());
}

//--------------------------------------------------------------
void ofApp::update(){
	realsense.update();
	if (realsense.isFrameNew())
	{
		//ofSetWindowShape(realsense.getDepthWidth() + realsense.getColorWidth(), realsense.getDepthHeight());
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0);
	ofPushMatrix();
	realsense.drawDepth();
	ofTranslate(realsense.getDepthWidth(), 0);
	realsense.drawColor();
	ofPopMatrix();
	ofDrawBitmapStringHighlight("fps:" + ofToString(ofGetFrameRate(), 2), ofGetWidth() - 160, ofGetHeight() - 20);
	gui.draw();
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

