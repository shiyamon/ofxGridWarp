//
//  ofxGridWarp.hpp
//  example
//
//  Created by Yoshida Shinya on 2022/09/26.
//

#pragma once
#include "ofMain.h"

class ofxGridWarp
{
public:
    enum gridType {
        ONE = 1,
        FOUR = 2,
        NINE = 3
    };
    
    void setup(int width, int height, gridType type = gridType::FOUR);
    void setupFromFile(string filename);
    void drawMesh(const ofTexture& tex);
    void resetMesh();
    void drawController();
    void enableController();
    void disableController();
    bool isControllerEnabled() { return _controllerEnabled; }
    void saveToFile(string filename);
    
    void mousePressed(ofMouseEventArgs &args);
    void mouseReleased(ofMouseEventArgs &args);
    void mouseMoved(ofMouseEventArgs &args);
    void mouseDragged(ofMouseEventArgs &args);
    void mouseScrolled(ofMouseEventArgs &args);
    void mouseEntered(ofMouseEventArgs &args);
    void mouseExited(ofMouseEventArgs &args);
    
    
private:
    const float POINT_RAD = 10.0f;
    const int   GRID_SUBDIV = 4;
    
    ofMesh              _mesh;
    vector<glm::vec3>   _ctrlPoints;
    vector<glm::vec3>   _verts;
    glm::mat4           _curModelView, _curView;
    
    gridType _gridType = gridType::FOUR;
    
    bool    _controllerEnabled = false;
    int     _pressedCtrlIndex = -1;
    int     _width, _height;
    int     _meshSubdiv;

    void updateGridMesh();
    bool mouseHitLocalSpace(glm::vec3 pt);
    glm::vec3 calcLocalSpaceMousePos();
    glm::vec3 calcScreenPos(glm::vec3 pt, glm::mat4 modelView);
};
