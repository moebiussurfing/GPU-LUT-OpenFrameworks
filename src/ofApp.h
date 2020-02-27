//modifications by moebiussurfing
//https://github.com/moebiussurfing
//using the code of HUBRIS mentioned below

/*
 ---------------------------------------------
 GPU LUT IN OPENFRAMEWORKS
 ---------------------------------------------
 BY HUBRIS [http://cargocollective.com/hubris]
 [http://github.com/youandhubris]
 PORTO á DECEMBER 2014
 ---------------------------------------------
 EXAMPLE
 ---------------------------------------------
 */

#pragma once

#include "ofMain.h"
#include "ofxSceneTEST.h"

class ofApp : public ofBaseApp {

public:

	ofxPanel gui;
	ofParameter<float> control1;
	ofParameter<float> control2;

	vector<string> lutPaths;
	vector<string> lutNames;

	int lutIndex;
	int numLuts;

	ofxSceneTEST scene;
	void setupLUT(std::string s);
	void setup();
	void setupFiles();

	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	//Shader
	ofShader lutFilter;
	ofPlanePrimitive plane;

	//LUT
	GLuint texture3D;
	int LUTsize = 32;
	string LUTpath;
	string LUTname;
	struct RGB { float r, g, b; };

	int LUT3dSize;// = 32;//most common

	ofFbo fbo;

	//vector< ofxGpuLut> luts;
	//ofImage lutImg;
	//ofVideoGrabber video;
	//bool isThumbnailView;
	//string description;
	//float thumbnailWidth, thumbnailHeight;
	//int numCols;
	//int numRows;
	//ofImage image;

	//struct RGB { float r, g, b; };
	//vector<RGB> LUT;
	
	////Video
	//ofVideoGrabber  webCam;
	//string filename;
};
