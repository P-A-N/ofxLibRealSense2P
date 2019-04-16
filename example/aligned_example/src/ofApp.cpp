#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	realsense.setupDevice(0);
	//setup different resolution
	realsense.enableDepth(1280, 720);
	realsense.enableColor(640, 480);
	realsense.startStream();

	//align depth and color pixel size
	realsense.setAligned(true);
}

//--------------------------------------------------------------
void ofApp::update(){
	realsense.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	//pixel size is aligned even if it setup different resolution!
	realsense.drawDepth(0, 0);
	realsense.drawColor(640, 0);
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
