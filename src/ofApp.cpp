#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    setupGUI();
    detection.setup(320,240);
    
    ofAddListener(detection.personDetected, this, &ofApp::personDetected);
    ofAddListener(detection.personLeft, this, &ofApp::personLeft);
}
//--------------------------------------------------------------
void ofApp::update()
{
    detection.update();
}
//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(50);
    detection.draw(10,10);
}
//--------------------------------------------------------------
void ofApp::personDetected(int &val)
{
    cout << "Person Detected: " << val << endl;
}
//--------------------------------------------------------------
void ofApp::personLeft(int &val)
{
    cout << "Person Left: " << val << endl;
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

}
//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
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

}
//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}
//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}
//--------------------------------------------------------------
// *
// * GUI
// *
//--------------------------------------------------------------
void ofApp::setupGUI()
{
    // Find the Files
    ofDirectory videoDirectory(ofToDataPath("videos",true));
    if (videoDirectory.exists()) {
        cout << "VideoDirectory: " << videoDirectory.path() << " Found" << endl;
        for(int file = 0; file < videoDirectory.getFiles().size(); file++) {
            videosInDirectory.push_back(videoDirectory.getFile(file).getAbsolutePath());
            videoNames.push_back(videoDirectory.getName(file));
        }
    }
    else{
        ofLogError() << "VideoDirectory: " << videoDirectory.path() << " MISSING";
    }
    
    // Setup the GUI
    gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
    gui->setTheme(new ofxDatGuiThemeSmoke());
    gui->addHeader("Watershed Segmentation");
    gui->addFRM(1.0f);
    gui->addBreak();
    gui->addDropdown("Video", videoNames);
    gui->addBreak();
    
    gui->addToggle("Show Output",false);
    
    gui->addButton("Calibrate");
    gui->addSlider("Calibration Frames", 1, 1000);
    vector<string> morphologyMode;
    morphologyMode.push_back("CV_MOP_ERODE");
    morphologyMode.push_back("CV_MOP_DILATE");
    morphologyMode.push_back("CV_MOP_OPEN");
    morphologyMode.push_back("CV_MOP_CLOSE");
    morphologyMode.push_back("CV_MOP_GRADIENT");
    morphologyMode.push_back("CV_MOP_TOPHAT");
    morphologyMode.push_back("CV_MOP_BLACKHAT");
    
    gui->addDropdown("Morphology", morphologyMode);
    
    gui->addSlider("Blur",0.0000,30,1);

    gui->addSlider("Noise Removal",0.0000,30,1);
    gui->addSlider("First Threshold",0,255,25);
    gui->addSlider("Second Threshold",0,255,25);
    gui->addSlider("Third Threshold",0,255,25);
    gui->addSlider("Dilation",0,15,1);
    gui->addSlider("Erosion",0,15,1);
    
    gui->addToggle("Equalize",false);
  
    gui->addBreak();

    gui->addSlider("Min Radius", 0, 320, 5);
    gui->addSlider("Max Radius", 0, 320, 5);
    gui->addSlider("Contour Threshold", 0, 255, 5);
    
    
    
    
    // Listeners
    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onTextInputEvent(this, &ofApp::onTextInputEvent);
    gui->on2dPadEvent(this, &ofApp::on2dPadEvent);
    gui->onDropdownEvent(this, &ofApp::onDropdownEvent);
    gui->onColorPickerEvent(this, &ofApp::onColorPickerEvent);
    gui->onMatrixEvent(this, &ofApp::onMatrixEvent);
}
//--------------------------------------------------------------
void ofApp::onSliderEvent(ofxDatGuiSliderEvent e)
{
    if (e.target->is("Blur")) {
        detection.setBlur(e.target->getValue());
    }
    else if (e.target->is("Dilation")) {
        detection.setDilateAmount(e.target->getValue());
    }
    else if (e.target->is("Erosion")) {
        detection.setErodeAmount(e.target->getValue());
    }
    else if (e.target->is("Noise Removal")) {
        detection.setNoiseRemoval(e.target->getValue());
    }
    else if (e.target->is("First Threshold")) {
        detection.setFirstThreshold(e.target->getValue());
    }
    else if (e.target->is("Second Threshold")) {
        detection.setSecondThreshold(e.target->getValue());
    }
    else if (e.target->is("Third Threshold")) {
        detection.setThirdThreshold(e.target->getValue());
    }
    else if (e.target->is("Min Radius")) {
        detection.setMinRadius(e.target->getValue());
    }
    else if (e.target->is("Max Radius")) {
        detection.setMaxRadius(e.target->getValue());
    }
    else if (e.target->is("Contour Threshold")) {
        detection.setContourThreshold(e.target->getValue());
    }
    else if (e.target->is("Calibration Frames")) {
        detection.setCalibrationLength(e.target->getValue());
    }
}
//--------------------------------------------------------------
void ofApp::onButtonEvent(ofxDatGuiButtonEvent e)
{
    if (e.target->is("Calibrate")) {
        detection.resetCalibration();
    }
    else if (e.target->is("Equalize")) {
        detection.useEqualizer(e.target->getEnabled());
    }
    else if (e.target->is("Show Output")) {
        detection.showResult(e.target->getEnabled());
    }
}
//--------------------------------------------------------------
void ofApp::onTextInputEvent(ofxDatGuiTextInputEvent e)
{
    
}
//--------------------------------------------------------------
void ofApp::on2dPadEvent(ofxDatGui2dPadEvent e)
{
    
    
}
//--------------------------------------------------------------
void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
    if(e.target->is("Video")) {
        detection.setVideo(videosInDirectory[e.child]);
    }
    else if (e.target->is("Morphology"))
    {
        detection.setMorphology(e.child);
        cout << e.target->getChildAt(e.child)->getName() << endl;
    }
}
//--------------------------------------------------------------
void ofApp::onColorPickerEvent(ofxDatGuiColorPickerEvent e)
{
    
}
//--------------------------------------------------------------
void ofApp::onMatrixEvent(ofxDatGuiMatrixEvent e)
{
    
}
