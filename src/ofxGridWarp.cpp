//
//  ofxGridWarp.cpp
//  example
//
//  Created by Yoshida Shinya on 2022/09/26.
//

#include "ofxGridWarp.hpp"
#include "glm/gtx/compatibility.hpp"

using namespace glm;

void ofxGridWarp::setup(int width, int height, gridType type)
{
    _width = width;
    _height = height;
    
    _gridType = type;
    int gridCnt = (int)_gridType;
    
    _meshSubdiv = GRID_SUBDIV * gridCnt;
    _meshSubdiv = GRID_SUBDIV * gridCnt;
    
    // generate mesh
    float dx = (float)_width/(float)_meshSubdiv;
    float dy = (float)_height/(float)_meshSubdiv;
    
    _mesh.clear();
    for( int y=0; y<=_meshSubdiv; ++y) {
        for( int x=0; x<=_meshSubdiv; ++x) {
            _mesh.addVertex(vec3(x*dx, y*dy, 0));
            _mesh.addTexCoord(vec2(x*dx, y*dy));
        }
    }
    
    for(int y=0; y<_meshSubdiv; ++y) {
        for(int x=0; x<_meshSubdiv; ++x) {
            int index = (_meshSubdiv+1)*y + x;
            int pt0 = index;
            int pt1 = pt0 + 1;
            int pt2 = index + _meshSubdiv + 1;
            int pt3 = pt2 + 1;
            _mesh.addIndex(pt0);
            _mesh.addIndex(pt2);
            _mesh.addIndex(pt1);
            _mesh.addIndex(pt1);
            _mesh.addIndex(pt2);
            _mesh.addIndex(pt3);
        }
    }
    
    resetMesh();
}


void ofxGridWarp::resetMesh()
{
    int gridCnt = (int)_gridType;
    float dx = _width / (float)gridCnt;
    float dy = _height / (float)gridCnt;
    
    _ctrlPoints.clear();
    for(int y=0; y<=gridCnt; ++y)
        for(int x=0; x<=gridCnt; ++x)
            _ctrlPoints.push_back(vec3(x*dx, y*dy, 0));
    
    updateGridMesh();
}


void ofxGridWarp::updateGridMesh()
{
    int gridCnt = (int)_gridType;
    int vCnt = _mesh.getVertices().size();
    
    for(int i=0; i<vCnt; ++i) {
        
        int cx = floor( (float)(i % (_meshSubdiv+1)) / (float)GRID_SUBDIV);
        int cy =  floor( floor((float)i / (float)(_meshSubdiv+1)) / (float)GRID_SUBDIV );
        float tx = (float)(i % (_meshSubdiv+1)) / (float)GRID_SUBDIV - cx;
        float ty = floor((float)i / (float)(_meshSubdiv+1)) / (float)GRID_SUBDIV - cy;
        
        int leftTopIdx = cy*(gridCnt+1) + cx;
        int rightTopIdx = leftTopIdx + 1;
        int leftBtmIdx = (cy+1)*(gridCnt+1) + cx;
        int rightBtmIdx = leftBtmIdx+1;
        
        int type=0;
        vec3 newPos = _ctrlPoints[leftTopIdx];
        if(cx < gridCnt && cy < gridCnt)
            newPos = lerp( lerp(_ctrlPoints[leftTopIdx], _ctrlPoints[rightTopIdx], tx), lerp(_ctrlPoints[leftBtmIdx], _ctrlPoints[rightBtmIdx], tx), ty);
        else if(cy < gridCnt)
            newPos = lerp(_ctrlPoints[leftTopIdx], _ctrlPoints[leftBtmIdx], ty);
        else if(cx < gridCnt)
            newPos = lerp(_ctrlPoints[leftTopIdx], _ctrlPoints[rightTopIdx], tx);
        
        _mesh.setVertex(i, newPos);
    }
}


void ofxGridWarp::drawMesh(const ofTexture &tex)
{
    tex.bind();
    _mesh.draw();
    tex.unbind();
}


void ofxGridWarp::drawController()
{
    // store current matrices
    _curModelView = ofGetCurrentMatrix(OF_MATRIX_MODELVIEW);
    _curView = ofGetCurrentViewMatrix();
    
    int gridCnt = (int)_gridType;
    
    ofPushStyle();
    ofSetColor(ofColor::red);
    for(int y=0; y<=gridCnt; ++y) {
        for(int x=0; x<=gridCnt; ++x) {
            int idx = x + y*(gridCnt+1);
            if(x<gridCnt)
                ofDrawLine(_ctrlPoints[idx], _ctrlPoints[idx+1]);
            if(y<gridCnt)
                ofDrawLine(_ctrlPoints[idx], _ctrlPoints[idx+gridCnt+1]);
        }
    }
    
    ofSetColor(ofColor::green);
    
    for(int i=0; i<_ctrlPoints.size(); ++i) {
        
        // dirty for now. should be cleaned up.
        if(_pressedCtrlIndex == i) {
            _ctrlPoints[i] = calcLocalSpaceMousePos();
            ofFill();
        }
        else if(mouseHitLocalSpace(_ctrlPoints[i]))
            ofFill();
        else ofNoFill();
        
        ofDrawCircle(_ctrlPoints[i], POINT_RAD);
    }
    
    ofPopStyle();
    
    updateGridMesh();
}


void ofxGridWarp::enableController()
{
    if(_controllerEnabled) return;
    ofRegisterMouseEvents(this);
    _controllerEnabled = true;
}


void ofxGridWarp::disableController()
{
    if(!_controllerEnabled) return;
    ofUnregisterMouseEvents(this);
    _controllerEnabled = false;
}


void ofxGridWarp::setupFromFile(string filename)
{
    if(!ofFile::doesFileExist(filename)){
        cout << "There is no file named: " + filename << endl;
        return;
    }
    
    ofBuffer buff = ofBufferFromFile(filename);
    string inTxt = buff.getText();
    vector<string> data = ofSplitString(buff.getText(), "_");
    _width = stoi(data[0]);
    _height = stoi(data[1]);
    _gridType = (gridType)stoi(data[2]);
    setup(_width, _height, _gridType);
    
    int offset=3;
    for(int i=0; i<_ctrlPoints.size(); ++i) {
        _ctrlPoints[i] = vec3(stoi(data[offset]), stoi(data[offset+1]), stoi(data[offset+2]));
        offset+=3;
    }
    updateGridMesh();
}


void ofxGridWarp::saveToFile(string filename)
{
    string outTxt = "";
    outTxt += ofToString(_width) + "_";
    outTxt += ofToString(_height) + "_";
    outTxt += ofToString((int)_gridType) + "_";
    for(vec3 pt : _ctrlPoints) {
        outTxt += ofToString(pt.x) + "_";
        outTxt += ofToString(pt.y) + "_";
        outTxt += ofToString(pt.z) + "_";
    }
    outTxt.pop_back();
    ofBuffer buff;
    buff.append(outTxt);
    ofBufferToFile(filename, buff);
}


bool ofxGridWarp::mouseHitLocalSpace(glm::vec3 pt)
{
    return glm::distance(pt, calcLocalSpaceMousePos()) < POINT_RAD;
}

vec3 ofxGridWarp::calcLocalSpaceMousePos()
{
    vec4 mousePos = vec4(ofGetMouseX(), ofGetMouseY(), 0, 1);
    return inverse(_curModelView) * _curView * mousePos;
}

vec3 ofxGridWarp::calcScreenPos(glm::vec3 pt, mat4 modelView)
{
    return glm::inverse(_curView) * _curModelView * vec4(pt, 1);
}


void ofxGridWarp::mousePressed(ofMouseEventArgs &args)
{
    for(int i=0; i<_ctrlPoints.size(); ++i) {
        if(mouseHitLocalSpace(_ctrlPoints[i])) {
            _pressedCtrlIndex = i;
            break;
        }
    }
}

void ofxGridWarp::mouseReleased(ofMouseEventArgs &args)
{
    _pressedCtrlIndex = -1;
}

void ofxGridWarp::mouseMoved(ofMouseEventArgs &args){}
void ofxGridWarp::mouseDragged(ofMouseEventArgs &args){}
void ofxGridWarp::mouseScrolled(ofMouseEventArgs &args){}
void ofxGridWarp::mouseEntered(ofMouseEventArgs &args){}
void ofxGridWarp::mouseExited(ofMouseEventArgs &args){}
