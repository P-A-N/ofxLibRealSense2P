#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	realsense.setupDevice();
	realsense.enableColor(640, 480);
	realsense.enableDepth(640, 480);
	realsense.startStream();
}

//--------------------------------------------------------------
void ofApp::update(){
	realsense.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(ofColor::white);
	realsense.drawColor(0, 0);
	ofSetColor(ofColor::yellow);
	ofDrawLine(ofGetMouseX(), 0, ofGetMouseX(), ofGetHeight());
	ofSetColor(ofColor::red);
	ofDrawLine(0, ofGetMouseY(), ofGetWidth(), ofGetMouseY());
	ofSetColor(ofColor::white);
	glm::vec3 point = realsense.getWorldCoordinateAt(ofGetMouseX(), ofGetMouseY());
	ofPushMatrix();
	ofTranslate(ofGetMouseX(), ofGetMouseY());
	ofDrawBitmapStringHighlight("screen coordinate : (" + ofToString(ofGetMouseX()) +", " + ofToString(ofGetMouseY()) + ")", 15, 15);
	ofDrawBitmapStringHighlight("world coordinate : (" + ofToString(point.x,2) + ", " + ofToString(point.y,2) + ", " + ofToString(point.z,2) + ")", 15, 30);
	ofPopMatrix();
	
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
