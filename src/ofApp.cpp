#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{

	//system("say 'hello world'");

	ofSetVerticalSync(true);

	threadedObject.start();
	threadedObject.draw();
	threadedObject.stop();


	font.loadFont("DIN.otf", 64);

	camWidth = 320;
	camHeight = 240;

	mode = MODE_HAAR;

	cam.initGrabber(camWidth, camHeight);

	

	opcClient.setup("127.0.0.1", 7890,2);
	//opcClientRight.setup("127.0.0.2", 7890);

	NeoPixelGrid8x8 grid;
	for (int i = 0; i < 8; i++){
		grid8x8Left.push_back(grid);
		grid8x8Right.push_back(grid);
		grid8x8Left[i].setupLedGrid();
		grid8x8Right[i].setupLedGrid();
		gridWidth = grid8x8Left[i].getWidth();// / 2;
		gridHeight = grid8x8Left[i].getHeight();// / 2;
	}

	soundStream.listDevices();
	int bufferSize = 256;
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	//volHistory.assign(400, 0.0);

	smoothedVol = 0.0;
	scaledVol = 0.0;
	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);


	fbo.allocate(closeUpScale, closeUpScale);
	//finder.setup("haarcascade_frontalface_default.xml");
	finder.setup("haarcascade_frontalface_default.xml");
	finderEye.setup("haarcascade_eye.xml");
	rect[INPUT].set(0, 0, ofGetWidth() / 2, ofGetHeight() / 2);
	rect[FILTER1].set(ofGetWidth() / 2, 0, ofGetWidth() / 2, ofGetHeight() / 2);
	rect[FILTER2].set(0, ofGetHeight() / 2, ofGetWidth() / 2, ofGetHeight() / 2);
	rect[OUTPUT].set(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth() / 2, ofGetHeight() / 2);
	margin = ofGetHeight() / 40;
	//mode = 0;

	gui.setup("control", "settings.xml", rect[0].x + margin, rect[0].y + (margin * 2));
	gui.add(closeUpScale.setup("subSectionScale", 100, 10, camWidth));
	gui.add(currentMode.setup("mode", ofToString(mode)));
	gui.add(sendOpc.setup("send opc"));

	
	gui.add(black.setup("black", 20, 0, 255));
	gui.add(white.setup("white", 90, 0, 255));
	gui.add(gamma.setup("gamma", 9.0, 0.01, 9.99));
	gui.add(useShader.setup("use Shader",false));
	gui.add(faceMargin.setup("face Margin", 0.2, 0.01, 1.00));
	gui.add(alphaShader.setup("alpha Shader", 1, 0, 255));

	gui.loadFromFile("settings.xml");
	//
	ofSetFrameRate(30);
	smoothFinder.set(0, 0, ofGetWidth() / 2, ofGetHeight() / 2);
	shader.load("noise");
	cout << "load" << endl;
}

//--------------------------------------------------------------
void ofApp::update()
{

ofSetWindowTitle(" FPS: " + ofToString((int)(ofGetFrameRate())));

	switch (mode)
	{
	case MODE_CAM:
	{
		cam.update();
		image.setFromPixels(cam.getPixels(), cam.getWidth(), cam.getHeight(), OF_IMAGE_COLOR);

	}
	break;
	case MODE_HAAR:
	{
		cam.update();
		image.setFromPixels(cam.getPixels(), cam.getWidth(), cam.getHeight(), OF_IMAGE_COLOR);
		finder.findHaarObjects(image);
	}
	break;
	case MODE_SOUND:
	{
		scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
		scaledVol *= 190.0f;
	}
	break;
	case MODE_EYE:
	{
		cam.update();
		image.setFromPixels(cam.getPixels(), cam.getWidth(), cam.getHeight(), OF_IMAGE_COLOR);
		finderEye.findHaarObjects(image);
	}
	break;

	}

	

	for (int j = 0; j < 2; j++){
		for (int i = 0; i < 4; i++){


			grid8x8Left[i + (j * 4)].update();
			grid8x8Right[i + (j * 4)].update();

			grid8x8Left[i + (j * 4)].grabImageData(ofPoint(margin + rect[FILTER2].x + gridWidth*j, margin + rect[FILTER2].y + i*gridHeight));
			grid8x8Right[i + (j * 4)].grabImageData(ofPoint(margin + rect[FILTER2].x + 2 * gridWidth + gridWidth*j, margin + rect[FILTER2].y + i*gridHeight));

		}
	}

	if (!opcClient.isConnected())
	{
		opcClient.tryConnecting();
	}
	else
	{

		opcClient.writeChannel(1, grid8x8Right[7].colorData());
		opcClient.writeChannel(2, grid8x8Right[5].colorData());
		opcClient.writeChannel(3, grid8x8Right[6].colorData());
		opcClient.writeChannel(4, grid8x8Right[4].colorData());

		opcClient.writeChannel(5, grid8x8Right[3].colorData());
		opcClient.writeChannel(6, grid8x8Right[2].colorData());
		opcClient.writeChannel(7, grid8x8Right[1].colorData());
		opcClient.writeChannel(8, grid8x8Right[0].colorData());




		opcClient.writeChannel(9, grid8x8Left[4].colorData());
		opcClient.writeChannel(10, grid8x8Left[5].colorData());
		opcClient.writeChannel(11, grid8x8Left[6].colorData());
		opcClient.writeChannel(12, grid8x8Left[7].colorData());

		opcClient.writeChannel(13, grid8x8Left[3].colorData());
		opcClient.writeChannel(14, grid8x8Left[2].colorData());
		opcClient.writeChannel(15, grid8x8Left[1].colorData());
		opcClient.writeChannel(16, grid8x8Left[0].colorData());



	}
	opcClient.update();

}

//--------------------------------------------------------------
void ofApp::draw()
{

	ofBackground(0);

	// As it says
	ofFill();
//	ofSetColor(ofMap(sin(ofGetElapsedTimeMillis()), -1, 1, 0, 255), ofMap(sin(ofGetElapsedTimeMillis() + 20), -1, 1, 0, 255), ofMap(sin(ofGetElapsedTimeMillis() + 10), -1, 1, 0, 255));
//	ofCircle(mouseX, mouseY, 30);

	ofSetColor(255, 255, 255);
	//drawEffects(effect);
	
	if (mode != MODE_SOUND) {
		cam.update();
		cam.draw(0, 0, camWidth, camHeight);
	}
	for (int i = 0; i<8; i++){
		grid8x8Left[i].drawGrabRegion(true);
		grid8x8Right[i].drawGrabRegion(true);
	}


	ofPushMatrix();
	ofTranslate(margin + rect[OUTPUT].x, margin + rect[OUTPUT].y);
	for (int j = 0; j < 2; j++){
		for (int i = 0; i < 4; i++){
			grid8x8Left[i + (j * 4)].drawGrid(gridWidth*j, i*gridHeight);
			grid8x8Right[i + (j * 4)].drawGrid(2 * gridWidth + gridWidth*j, i*gridHeight);
		}
	}
	ofPopMatrix();

	
	ofSetColor(255, 255, 255);
	switch (mode)
	{
	case MODE_CAM:
	{

		//1
		image.draw(0, 0);
		image.draw(rect[FILTER2].x + margin, rect[FILTER2].y + margin);
		ofNoFill();
		ofSetColor(ofColor::blue);
		/*
		ofRect(mouseX, mouseY, closeUpScale, closeUpScale);

		//2
		if (prevScaleForAllocate != closeUpScale)
		{
			fbo.allocate(closeUpScale, closeUpScale);
			prevScaleForAllocate = closeUpScale;
		}
		fbo.begin();
		ofSetColor(255, 255, 255);
		image.drawSubsection(0, 0, closeUpScale, closeUpScale, mouseX, mouseY);
		fbo.end();
		ofPushMatrix();
		ofTranslate(rect[1].x, rect[1].y);
		ofScale(2, 2);
		fbo.draw(0, 0, fbo.getWidth(), fbo.getHeight());
		ofPopMatrix();
		*/

	}
	break;
	case MODE_HAAR: case  MODE_EYE:
	{
		//1
		image.draw(0, 0);
		ofNoFill();
		ofSetColor(ofColor::green);
		ofRectangle cur(0,0,0,0);
		
			int indexSelection=0;
			int size = 0;
			if (mode == MODE_HAAR){
			for (unsigned int i = 0; i < finder.blobs.size(); i++)
			{
					cur = finder.blobs[i].boundingRect;
					ofRect(cur);
				
					if (cur.width + cur.height > size) {
						size = cur.width + cur.height;
						indexSelection = i;
					}
				}
				if (finder.blobs.size() > 0) {
				cur = finder.blobs[indexSelection].boundingRect;
				prevFinder = cur;
				}
				else {
					cur = prevFinder;
				}
			}
			else if (mode == MODE_EYE) {
				for (unsigned int i = 0; i < finderEye.blobs.size(); i++)
				{
					cur = finderEye.blobs[i].boundingRect;
					ofRect(cur);

					if (cur.width + cur.height > size) {
						size = cur.width + cur.height;
						indexSelection = i;
					}
				}
				if (finderEye.blobs.size() > 0) {
					cur = finderEye.blobs[indexSelection].boundingRect;
					prevFinder = cur;
				}
				else {
					cur = prevFinder;
				}
			}

			if ((cur.width == cur.height) && cur.width>0)
			{

				smoothFinder.x += ((cur.x - smoothFinder.x)*0.05);
				smoothFinder.y += ((cur.y - smoothFinder.y)*0.05);

				smoothFinder.width += ((cur.width - smoothFinder.width)*0.1);
				smoothFinder.height += ((cur.height - smoothFinder.height)*0.1);

				ofSetColor(0, 0, 255);
				ofRect(smoothFinder);
				closeUpScale = smoothFinder.width;
				//if (prevScaleForAllocate != closeUpScale)
				//{
				//	fbo.allocate(closeUpScale, closeUpScale);
				//	prevScaleForAllocate = closeUpScale;
				//}
				//
				ofSetColor(255, 255, 255,255);
				image.drawSubsection(rect[FILTER1].x, rect[FILTER1].y, smoothFinder.width, smoothFinder.height, smoothFinder.x, smoothFinder.y);
				float fMargin = smoothFinder.width*faceMargin;
			//fbo.begin();
				ofEnableAlphaBlending();
				ofSetColor(255,255,255, 255);
				if (useShader){
				shader.begin();
				shader.setUniform1f("inGamma", gamma);
				shader.setUniform1i("inBlack", black);
				shader.setUniform1i("inWhite", white);
				image.drawSubsection(margin + rect[FILTER2].x, margin + rect[FILTER2].y, panelWIDTH, panelWIDTH, smoothFinder.x - fMargin, smoothFinder.y - fMargin, smoothFinder.width + (fMargin * 2), smoothFinder.height + (fMargin * 2));
				shader.end();
				}		
				
				ofSetColor(255, 255, 255, alphaShader);
				image.drawSubsection(margin + rect[FILTER2].x, margin + rect[FILTER2].y, panelWIDTH, panelWIDTH, smoothFinder.x - fMargin, smoothFinder.y - fMargin, smoothFinder.width + (fMargin * 2), smoothFinder.height + (fMargin * 2));
			
			}
		

	}
	break;
	case MODE_SOUND:
	{

		//1

		int vSize = 20;
		//cout << "scaledVol" << scaledVol << endl;
		float currentSize = ofMap(scaledVol, 0, 80, 0, vSize, true);
		//ofSetColor(0,0,0,255);
		//ofRect(0,0,fbo.getWidth(),fbo.getHeight());
		//ofClear(0);
		fbo.begin();
		ofSetColor(255, 255, 255, 255);
		for (int i = 0; i<(int)currentSize; i++)
		{
			ofSetLineWidth(ofMap(scaledVol, 0, 100, 0, 80, true) / i);
			ofCircle(rect[FILTER2].height / 2, rect[FILTER2].height / 2, ofMap(i, 0, vSize, 2, rect[FILTER2].height / 2));
		}
		fbo.end();
		fbo.draw(0, 0);
		fbo.draw(rect[FILTER2].x, rect[FILTER2].y);
		ofSetLineWidth(1.0);
	
	}
	break;

	}

		//UI ELEMENTS
		ofNoFill();
		ofSetColor(240, 240, 240);
		for (int i = 0; i<4; i++)
		{
			ofRect(rect[i]);

		}
		ofPushMatrix();
		ofTranslate(margin, margin);
		ofDrawBitmapStringHighlight("Raw Input", rect[INPUT].x, rect[INPUT].y);
		ofDrawBitmapStringHighlight("Filter1", rect[FILTER1].x, rect[FILTER1].y);
		ofDrawBitmapStringHighlight("Filter2", rect[FILTER2].x, rect[FILTER2].y);
		ofDrawBitmapStringHighlight("Output", rect[OUTPUT].x, rect[OUTPUT].y);
		ofPopMatrix();
		ofSetColor(255, 255, 255);
		gui.draw();

		/*
	}
	else
	{
		currentMode = "fbo not allocated";
	}
		*/
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key = ' ')
	{
		mode++;
		currentMode = ofToString(mode);
		if (mode>MODE_SOUND)
		{
			mode = 0;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
//	bSendSerialMessage = true;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
	rect[INPUT].set(0, 0, ofGetWidth() / 2, ofGetHeight() / 2);
	rect[FILTER1].set(ofGetWidth() / 2, 0, ofGetWidth() / 2, ofGetHeight() / 2);
	rect[FILTER2].set(0, ofGetHeight() / 2, ofGetWidth() / 2, ofGetHeight() / 2);
	rect[OUTPUT].set(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth() / 2, ofGetHeight() / 2);
	margin = ofGetHeight() / 40;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}

void ofApp::exit()
{
	gui.saveToFile("settings.xml");
}
void ofApp::audioIn(float * input, int bufferSize, int nChannels)
{

	float curVol = 0.0;

	// samples are "interleaved"
	int numCounted = 0;

	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume
	for (int i = 0; i < bufferSize; i++)
	{
		left[i] = input[i * 2] * 0.5;
		right[i] = input[i * 2 + 1] * 0.5;

		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted += 2;
	}

	//this is how we get the mean of rms :)
	curVol /= (float)numCounted;

	// this is how we get the root of rms :)
	curVol = sqrt(curVol);

	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;

	//bufferCounter++;

}

