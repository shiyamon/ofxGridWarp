#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    _image.load("check256.png");
    
    if(ofFile::doesFileExist("gridWarp.txt"))
        _gridWarp.setupFromFile("gridWarp.txt");
    else
        _gridWarp.setup(_image.getWidth(),_image.getHeight());
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0);
    
    ofPushMatrix();
    ofTranslate(100, 100, 0);
//    ofScale(0.5f, 0.5f);
    ofScale(2.0f, 2.0f);
    _gridWarp.drawMesh(_image.getTexture());
    if(_gridWarp.isControllerEnabled())
        _gridWarp.drawController();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if(key == 'c') {
        if(_gridWarp.isControllerEnabled())
            _gridWarp.disableController();
        else
            _gridWarp.enableController();
    }
    
    if(key == 's') {
        _gridWarp.saveToFile("gridWarp.txt");
    }
}
