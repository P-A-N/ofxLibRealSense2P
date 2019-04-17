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
