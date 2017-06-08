#include "ofApp.h"
#include <GLUT/GLUT.h>
int xSize           = 1024;
int ySize           = 768;
int xMid            = xSize/2;
int yMid            = ySize/2;
int xRes            = 60;
int yRes            = 60;
//float _decay		= .991;
float _decay		= 0.98;
int res             = 15;
float heightMulti = 1;
float xm = 0, ym = 0;
float xp = 0, yp = 0, zp = 0;
float xa = 0, ya = 0, za = 0;
float xb = 0, yb = 0, zb = 0;
float elevation;
float azimuth;
float _distance;
bool lighting = true;
float angle = 0;
string msg = "press arraw key to rotat\npress space to toggle shader\nframerate :";
void ofApp::findRipples(){
    for (int y=1; y<yRes-1; y++){
        for (int x=1; x<xRes-1; x++){
            r0[x][y] = (r1[x-1][y] + r1[x+1][y] + r1[x][y-1] + r1[x][y+1]) / 4.0;
            r0[x][y] = r0[x][y] * 2.0 - r2[x][y];
            r0[x][y] *= _decay;
        }
    }
}

void ofApp::swapBuffers(){
    for (int y=0; y<yRes; y++){
        for (int x=0; x<xRes; x++){
            r2[x][y] -= (r2[x][y] - r1[x][y]) * _decay;
            r1[x][y] -= (r1[x][y] - r0[x][y]) * _decay;
        }
    }
}
void ofApp::render(){
    float _height = yRes*res;
    float _width = xRes*res;
    for (int y=2; y<yRes; y++){
        glBegin(GL_QUAD_STRIP);
        for (int x=1; x<xRes; x++){
            xp = (x - xRes/2) * res - _height * 0.5;
            yp = (y - yRes/2) * res - _width * 0.5;
            zp = r1[x][y] * heightMulti;
            
            if (lighting){
                ya = (y - yRes/2) * res;
                za = r1[x][y] * heightMulti;
                yb = ((y-1) - yRes/2) * res;
                zb = r1[x-1][y-1] * heightMulti;
                
                angle = ofVec2f(ya, za).angle(ofVec2f( yb, zb))*125.0 + 125;
                ofFill();
                ofSetColor(angle/2.0 + 125,50,angle);
            } else {
                ofFill();
                ofSetColor(r0[x][y]/5.0 + 125.0);
            }
            glNormal3f(xp, yp, zp);
            glVertex3f(xp, yp, zp);
            
            xp = (x - xRes/2) * res - _width * 0.5;
            yp = ((y-1) - yRes/2) * res - _height * 0.5;
            zp = r1[x][y-1] * heightMulti;
            if (lighting){
                ya = ((y-1) - yRes/2) * res;
                za = r1[x][y-1] * heightMulti;
                yb = ((y-2) - yRes/2) * res;
                zb = r1[x-1][y-2] * heightMulti;
                
                angle = ofVec2f(ya, za).angle(ofVec2f( yb, zb))*125.0 + 125;
                ofFill();
                ofSetColor(angle/2.0 + 125,50,angle);
                
            } else {
                ofFill();
                ofSetColor(r0[x][y-1]/5.0 + 125.0);
            }
            glNormal3f(xp, yp, zp);
            //            glNormal3f(lightDir);
            glVertex3f(xp, yp, zp);
        }
        glEnd();
    }
}
//--------------------------------------------------------------
void ofApp::makeRipples(float _x, float  _y){
    xm = ofClamp((int)(_x/(xSize/xRes)),0,xRes-1);
    ym = ofClamp((int)(_y/(ySize/yRes)),0,xRes-1);
//    std::printf("x: %.0f", xm);
//    std::printf(" y: %.0f\n", ym);
    for (int y=1; y<yRes-1; y++){
        for (int x=1; x<xRes-1; x++){
            float d = ofDist(xm,ym,x,y);
            if (d < 3){
                r1[x][y] -= pow(((3 - d)/3.0),2) * 200.0;
                //                std::printf("r1: %f\n", r1);
            }
        }
    }
}
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    glEnable(GL_DEPTH_TEST);
    ofSetWindowShape(xSize,ySize);
    ofSetFrameRate(60);
    //ofHideCursor();
    //pov = POV(200);
    r0 = new float* [xRes];
    r1 = new float* [xRes];
    r2 = new float* [xRes];
    
    for(int y = 0 ; y < yRes ; y++)
    {
        r0[y] = new float [yRes];
        r1[y] = new float [yRes];
        r2[y] = new float [yRes];
        for(int x = 0 ; x < xRes ; x++)
        {
            r0[y][x] = 0;
            r1[y][x] = 0;
            r2[y][x] = 0;
        }
    }
    
    shader.load("shaders/toon1.vert","shaders/toon1.frag");
    _distance     = 100;
    azimuth      = 0.0;
    elevation    = 0.0;
    lightDir.z = 10;
    light.setPosition(lightDir.x, lightDir.y, lightDir.z);
    light.setDiffuseColor( ofColor(255));
    light.setPointLight();
    light.enable();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    findRipples();
    swapBuffers();
}

//--------------------------------------------------------------
void ofApp::draw(){
    cam.begin();
    glEnable(GL_DEPTH_TEST);
    light.enable();
    if(lighting){
        shader.begin();
        ofVec3f nl = lightDir.getNormalized();
        shader.setUniform3f("lightDir",nl.x,nl.y,nl.z);
    }
    ofPushMatrix();
    ofTranslate(xMid,yMid,_distance);
    ofRotate(elevation,1,0,0);
    ofRotate(azimuth,0,0,1);
    render();
        glutSolidTeapot(10);
    ofPopMatrix();
    
    if(lighting)shader.end();
    ofPushStyle();
    ofSetColor(ofColor::yellow);
    ofDrawCone(lightDir.x, lightDir.y, lightDir.z, 50, 50);
    ofPopStyle();
    light.disable();
    
    glDisable(GL_DEPTH_TEST);
    cam.end();
    ofSetColor(255);
    ofDrawBitmapString(msg+ofToString(ofGetFrameRate()),20,20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == OF_KEY_UP){
        elevation += 1;
    } else if (key == OF_KEY_DOWN){
        elevation -= 1;
    } else if (key == OF_KEY_RIGHT){
        azimuth -= 1;
    } else if (key == OF_KEY_LEFT){
        azimuth += 1;
    }
    if (key == ' '){
        lighting = !lighting;
    }
    if(key == 'a'){
        lightDir.x -= 50;
    }
    if(key == 'd'){
        lightDir.x += 50;
    }
    if(key == 'w'){
        lightDir.y -= 50;
    }
    if(key == 's'){
        lightDir.y += 50;
    }
    
    if(key == 'q'){
        lightDir.z -= 50;
    }
    if(key == 'e'){
        lightDir.z += 50;
    }
    light.setPosition(lightDir.x, lightDir.y, lightDir.z);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    makeRipples(x,y);
}

void ofApp::mouseMoved(int x, int y){
    //    makeRipples(x,y);
}
