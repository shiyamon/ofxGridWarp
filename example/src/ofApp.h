#pragma once

#include "ofMain.h"
#include "ofxGridWarp.hpp"

class ofApp : public ofBaseApp
{
    
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    
    ofImage _image;
    ofxGridWarp _gridWarp;
};
