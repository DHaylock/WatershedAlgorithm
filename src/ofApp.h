#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "Detection_Abstraction.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void personDetected(int &val);
        void personLeft(int &val);
    
        Detection_Abstraction detection;
    
        vector<string> videosInDirectory;
        vector<string> videoNames;
    
        // GUI Objects
        ofxDatGui * gui;
        void onButtonEvent(ofxDatGuiButtonEvent e);
        void onSliderEvent(ofxDatGuiSliderEvent e);
        void onTextInputEvent(ofxDatGuiTextInputEvent e);
        void on2dPadEvent(ofxDatGui2dPadEvent e);
        void onDropdownEvent(ofxDatGuiDropdownEvent e);
        void onColorPickerEvent(ofxDatGuiColorPickerEvent e);
        void onMatrixEvent(ofxDatGuiMatrixEvent e);
        void setupGUI();
};
