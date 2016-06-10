#pragma once


#include "ofMain.h"
#include "ofxGui.h"
#define gridW 8
#define gridH 8
#define panelNumW 3
#define panelNumH 3
#define panelWIDTH 243
#include "ofxCvHaarFinder.h"
#include "ofxOPC.h"
#include "threadedObject.h" // include our ThreadedObject class.



#define MODE_CAM 0
#define MODE_HAAR 1
#define MODE_EYE 2
#define MODE_SOUND 3


#define INPUT 0
#define FILTER1 1
#define FILTER2 2
#define OUTPUT 3

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofTrueTypeFont		font;

	ofxOPC opcClient;
	ofxOPC opcClientRight;
	vector<NeoPixelGrid8x8>grid8x8Left;
	vector<NeoPixelGrid8x8>grid8x8Right;
	int gridWidth;
	int gridHeight;

	bool hide;

	ofVideoGrabber cam;

	//ofVideoGrabber cam;

	int camWidth;
	int camHeight;

	ofFbo fbo;

	ofImage image;

	int closeUpW;
	int closeUpH;
	ofxFloatSlider closeUpScale;
	float prevScaleForAllocate;
	ofxToggle sendOpc;
	ofxToggle nextMode;
	
	int mode;

	ofxFloatSlider gamma;
	ofxIntSlider white;
	ofxIntSlider    black;
	ofxIntSlider    alphaShader;
	ofxFloatSlider    faceMargin;
	ofxToggle useShader;
	ofxToggle stayInvisible;

	float fboDrawScale;// = 50;
	ofxPanel gui;
	ofxLabel currentMode;

	ofxCvHaarFinder finder;
	ofxCvHaarFinder finderEye;

	ofRectangle smoothFinder;
	ofRectangle prevFinder;

	ofRectangle rect[4];

	float margin;

	void audioIn(float * input, int bufferSize, int nChannels);

	vector <float> left;
	vector <float> right;
	//vector <float> volHistory;

	//int 	bufferCounter;
	//int 	drawCounter;

	float smoothedVol;
	float scaledVol;
	ofSoundStream soundStream;

	ThreadedObject threadedObject;

	ofShader shader;

};

