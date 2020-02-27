#include "ofApp.h"
//--------------------------------------------------------------
void ofApp::setupFiles()
{
	lutIndex = 0;

	ofDirectory dir;
	dir.allowExt("cube");
	dir.allowExt("CUBE");
	dir.listDir("LUT");
	dir.sort();
	numLuts = dir.size();
	ofLogNotice(__FUNCTION__) << "num LUTs:" << numLuts;

	lutNames.resize(numLuts);
	lutPaths.resize(numLuts);
	//luts.resize(numLuts);

	for (int i = 0; i < numLuts; i++)
	{
		std::string lutPath = dir.getPath(i);
		lutPaths[i] = lutPath;

		ofStringReplace(lutPath, "LUT", "");
		ofStringReplace(lutPath, ".cube", "");
		ofStringReplace(lutPath, ".CUBE", "");
		ofStringReplace(lutPath, "\"", "");
		lutNames[i] = lutPath;

		ofLogNotice(__FUNCTION__) << "lutPaths[" << i << "]: " << lutPaths[i];
		ofLogNotice(__FUNCTION__) << "lutNames[" << i << "]: " << lutNames[i];
	}

	//--

	//description = "space key : switch thumbnail / full-size view\n";
	//description += "left key : previous filter\n";
	//description += "right key : next filter\n";

	//video.initGrabber(1920, 1080);

	////source image
	////image.load("img/background3.jpg");
	////image.load("img/photo3.jpg");
	////image.load("img/photo5.jpg");
	//image.load("img/pic1.jpg");
	////image.load("img/pic2.png");
	////image.load("img/pic3.jpg");

	//isThumbnailView = true;
	//lutIndex = 0;

	//numCols = 4;
	//numRows = 4;
	////thumbnailWidth = 1920 / (dirSize/ numCols);
	////thumbnailHeight = 1080 / (dirSize / numRows);
	//thumbnailWidth = 1920 / numCols;
	//thumbnailHeight = 1080 / numRows;
}

//--------------------------------------------------------------
void ofApp::setupLUT(std::string s) 
{
	ofLogNotice(__FUNCTION__) << "lutIndex:" << lutIndex;
	ofLogNotice(__FUNCTION__) << "lutPaths[lutIndex]:" << lutPaths[lutIndex];
	ofLogNotice(__FUNCTION__) << "lutNames[lutIndex]:" << lutNames[lutIndex];
	LUTname = lutNames[lutIndex];

	//-

	LUTpath = ofToDataPath(s);
	ifstream LUTfile(LUTpath.c_str());

	vector<RGB> LUT;

	//-

	const string key_LUT_3D_SIZE = "LUT_3D_SIZE";

	//multiple pow2 lut sizes: 16, 32, 64
	LUT3dSize = 32;//most common
	while (!LUTfile.eof()) 
	{
		string row;
		getline(LUTfile, row);

		if (row.empty()) continue;

		RGB line;
		if (sscanf(row.c_str(), "%f %f %f", &line.r, &line.g, &line.b) == 3) {
			LUT.push_back(line);
		}
		else if (ofIsStringInString(row, key_LUT_3D_SIZE)) {
			vector<string> subs = ofSplitString(ofTrim(row), " ");
			if (subs.size() >= 2) {
				ofLogNotice(__FUNCTION__) << "found key_LUT_3D_SIZE: " << subs[1];
				LUT3dSize = ofToInt(subs[1]);
			}
		}
	}//while

	if (LUT.size() != (pow(LUT3dSize, 3.0))) 
	{
		ofLogError(__FUNCTION__) << "LUT size is incorrect.";
		return;//skip load this lut
		//std::exit(1);
	}
	else if (ofNextPow2(LUT3dSize) != LUT3dSize) 
	{
		ofLogError(__FUNCTION__) << "LUT needs to be pow2.";
		return;//skip load this lut
		//std::exit(1);
	}

	ofLogNotice(__FUNCTION__) << "LUT.size(): " << LUT.size() << " --> " << int(ceil(pow(LUT.size(), 1.0 / 3.0)));
	ofLogNotice(__FUNCTION__) << "LUT3dSize: " << LUT3dSize;

	//-

	//fixed lut size 32
	//while (!LUTfile.eof()) {
	//	string LUTline;
	//	getline(LUTfile, LUTline);
	//	if (LUTline.empty()) continue;
	//	RGB line;
	//	if (sscanf(LUTline.c_str(), "%f %f %f", &line.r, &line.g, &line.b) == 3) LUT.push_back(line);
	//}
	//if (LUT.size() != (pow(LUTsize, 3.0)))
	//{
	//	ofLogError() << "LUT size is incorrect.";
	//	return;
	//}

	//--

	ofSetWindowTitle(LUTname);

	//-

	//disable rectangle textures
	ofDisableArbTex();

	//-

	//create a 3D texture
	//reference from http://content.gpwiki.org/index.php/OpenGL:Tutorials:3D_Textures
	glEnable(GL_TEXTURE_3D);
	glGenTextures(1, &texture3D);
	glBindTexture(GL_TEXTURE_3D, texture3D);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, LUT3dSize, LUT3dSize, LUT3dSize, 0, GL_RGB,
		GL_FLOAT, &LUT[0]);
	//glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, LUTsize, LUTsize, LUTsize, 0, GL_RGB,
	//	GL_FLOAT, &LUT[0]);
	//LUT3dSize now

	glBindTexture(GL_TEXTURE_3D, 0);
	glDisable(GL_TEXTURE_3D);

	//-

	//ofEnableArbTex();
}

//--------------------------------------------------------------
void ofApp::setup() 
{
	//gui
	control1.set("MIX", 1, 0, 1);
	control2.set("control2", 1, 0, 1);//not used in shader
	gui.setup("LUT");
	gui.add(control1);
	//gui.add(control2);//not used in shader

	//-

	//load the shader
	lutFilter.load("Shaders/ShaderLUT/lut_filter");

	//read lut files folder
	setupFiles();

	//load first LUT file
	lutIndex = 0;
	LUTname = lutNames[lutIndex];
	setupLUT(lutPaths[lutIndex]);

	//--

	//source image
	//start the webCam and set a plane to texture
	//webCam.initGrabber(1280, 720);

	fbo.allocate(ofGetWidth(), ofGetHeight());

	plane.set(ofGetWidth(), ofGetHeight(), 2, 2);
	plane.setPosition(ofGetWidth() / 2, ofGetHeight() / 2, 0);
}

//--------------------------------------------------------------
void ofApp::update() 
{
	//update sources
	//webCam.update();

	fbo.begin();
	ofClear(0, 0);
	scene.drawAll();
	fbo.end();
}

//--------------------------------------------------------------
void ofApp::draw() 
{
	//ofEnableArbTex();//not required

	//-

	//run lut shader
	lutFilter.begin();

	//source
	//lutFilter.setUniformTexture("tex", webCam.getTextureReference(), 0);
	lutFilter.setUniformTexture("tex", fbo.getTextureReference(), 0);

	lutFilter.setUniformTexture("lutTexure", GL_TEXTURE_3D, texture3D, 1);
	//lutFilter.setUniform1f("lutSize", LUTsize);
	lutFilter.setUniform1f("lutSize", LUT3dSize);
	
	//control
	lutFilter.setUniform2f("mouse", control1.get(), control2.get());
	//lutFilter.setUniform2f("mouse", (float)mouseX / ofGetWidth(), (float)mouseY / ofGetHeight());

	plane.draw();

	lutFilter.end();

	//-

	//gui
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) 
{
	//browse luts
	if (key == OF_KEY_DOWN)
	{
		lutIndex++;
		if (lutIndex >= numLuts)
			lutIndex = 0;
		setupLUT(lutPaths[lutIndex]);
	}
	else if (key == OF_KEY_UP)
	{
		lutIndex--;
		if (lutIndex <= 0)
			lutIndex = numLuts-1;
		setupLUT(lutPaths[lutIndex]);
	}

	//--

	if (key == 's')
	{
		bool b = !scene.getGuiVisible();
		scene.setGuiVisible(b);
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	fbo.allocate(ofGetWidth(), ofGetHeight());

	plane.set(ofGetWidth(), ofGetHeight(), 2, 2);
	plane.setPosition(ofGetWidth() / 2, ofGetHeight() / 2, 0);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
