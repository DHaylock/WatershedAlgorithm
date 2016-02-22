//
//  Detection_Abstraction.cpp
//  Watershed Algorithm
//
//  Created by David Haylock on 01/02/2016.
//
//

#include "Detection_Abstraction.h"
//--------------------------------------------------------------
void Detection_Abstraction::setVideo(string filePath)
{
//    videoPlayer.closeMovie();
//    videoPlayer.close();
    string videoPath = ofToDataPath(filePath, true);
    
    videoPlayer.load(videoPath);
    videoPlayer.play();
    
    ofxCv::imitate(abDifference, videoPlayer);
    ofxCv::imitate(abDifferenceMutated, videoPlayer);
    abDifferenceMutated = cv::Mat(videoPlayer.getHeight(),videoPlayer.getWidth(),CV_8UC1);
    mutatedMat = cv::Mat(videoPlayer.getHeight(),videoPlayer.getWidth(),CV_8UC1);
    for (int y = 0; y < videoPlayer.getHeight(); y++) {
        for (int x = 0; x < videoPlayer.getWidth(); x++) {
            abDifferenceMutated.at<uchar>(y,x) = 0;
            mutatedMat.at<uchar>(y,x) = 0;
        }
    }
    curFlow = &fb;
}
//--------------------------------------------------------------
void Detection_Abstraction::setup(int width, int height)
{
    font.load("ofxdatgui_assets/font-verdana.ttf", 10);
    
    // Initial Vide
    setVideo("videos/IRCapture.mp4");
    
    contourFinder.setMinAreaRadius(10);
    contourFinder.setMaxAreaRadius(50);
    subFinder.setMinAreaRadius(10);
    subFinder.setMaxAreaRadius(50);
    
    calibrationLength = 30;
    frames = 0;
    blurAmount = 7 ;
    alphaBlend = 0.01 ;
    firstThresholdValue = 25;
    secondThresholdValue = 25;
    thirdThresholdValue = 25;
    
    customThresholdValue = 50;
    numberOfPeopleDetected = 0;
    dilationAmount = 3;
    erosionAmount = 3;
    noiseRemoval = 3;
    equalize = false;
    showOutput = false;
    morphology = cv::MORPH_GRADIENT;
    
    bgs = new cv::BackgroundSubtractorMOG2(1000,10,false);
}
//--------------------------------------------------------------
void Detection_Abstraction::update()
{
    videoPlayer.update();
    if(videoPlayer.isFrameNew())
    {
        frameMat = ofxCv::toCv(videoPlayer);
        ofxCv::convertColor(frameMat, frameGray, CV_RGB2GRAY);
        
        // Try this erode
        ofxCv::erode(frameGray,3);
        
        if(equalize) {
            cv::equalizeHist(frameGray, frameGray);
        }
        
        // Blur Values can only be odd
        if (noiseRemoval % 2 == 0) {
            noiseRemoval++;
        }
        
        // Cheap Way to Remove Noise
        ofxCv::GaussianBlur(frameGray, noiseRemoval);
        
        // Calibration Loop
        if (frames < calibrationLength) {
            if (frames == 0) {
                // Make a New Mat set it to black
                background = cv::Mat(frameMat.rows,frameMat.cols,CV_8UC1);
                for (int y = 0; y < background.rows; y++) {
                    for (int x = 0; x < background.cols; x++) {
                        background.at<uchar>(y,x) = 0;
                    }
                }
            }
            // Then loop through the background pixels setting the bac
            for (int y = 0; y < background.rows; y++) {
                for (int x = 0; x < background.cols; x++) {
                    unsigned char backPixel = background.at<uchar>(y, x);
                    unsigned char currentPixel = frameGray.at<uchar>(y, x);
                    
                    if (currentPixel == 0) {
                        backPixel = 0;
                    }
                    else if (currentPixel > backPixel) {
                        backPixel = currentPixel;
                    }
                    
                    background.at<uchar>(y, x) = backPixel;
                }
            }
            
            frames++;
        }
        // Resolve the calibration loop
        else if (frames == calibrationLength) {
            cv::Scalar th = cv::mean(background);
            meanOfMatrix = (int)(th[0]);
            cout << meanOfMatrix << endl;
            frames = calibrationLength+1;
        }
        else {
            cv::absdiff(frameGray, background, abDifference);
            ofxCv::threshold(abDifference, firstThresholdValue);
            
            ofxCv::GaussianBlur(abDifference, blurAmount);
            
            subFinder.setFindHoles(false);
        
            cv::Mat element50(3,3,CV_8U,cv::Scalar(1));
            cv::morphologyEx(abDifference, abDifference, morphology, element50);
            
            ofxCv::threshold(abDifference, abDifferenceMutated, secondThresholdValue);
            
            ofxCv::GaussianBlur(abDifferenceMutated, 21);
            
            subFinder.findContours(abDifferenceMutated);
        
            // Erode
            cv::erode(abDifferenceMutated,erodedMat,cv::Mat(),cv::Point(-1,-1),erosionAmount);

            // Dilate
            cv::dilate(abDifferenceMutated, dilatedMat, cv::Mat(),cv::Point(-1,-1),dilationAmount);
            
            // Threshold the Dilated Image
            cv::threshold(dilatedMat, dilatedMat, 1, thirdThresholdValue, cv::THRESH_BINARY_INV);
            
            // Create the new marker Matrix
            markers = cv::Mat(abDifferenceMutated.rows, abDifferenceMutated.cols, CV_8UC3,cv::Scalar(0));
            ofxCv::imitate(watershedMat, markers);

            // Add the Matrices together
            markers = erodedMat+dilatedMat;
            
            for (int y = 0; y < markers.rows; y++) {
                for (int x = 0; x < markers.cols; x++) {
                    unsigned char backPixel = mutatedMat.at<uchar>(y,x);
                    unsigned char currentPixel = markers.at<uchar>(y,x);

                    if (currentPixel == firstThresholdValue) {
                        backPixel = firstThresholdValue;
                    }
                    else if(currentPixel == 0) {
                        backPixel = 255;
                    }
                    else {
                        backPixel = 0;
                    }
                    mutatedMat.at<uchar>(y,x) = backPixel;
                }
            }
            contourFinder.setFindHoles(false);
            segmenter.setMarkers(markers);
            watershedMat = segmenter.process(frameMat);
            watershedMat.convertTo(watershedMat, CV_8U);
            
            ofxCv::threshold(watershedMat, firstThresholdValue);
            ofxCv::GaussianBlur(watershedMat, 21);
            contourFinder.findContours(watershedMat);
        }


    }
    

    int as = 1;
    numberOfPeopleDetected = contourFinder.size();
    if (numberOfPeopleDetected < preNumberOfPeopleDetected) {
        ofNotifyEvent(personLeft,numberOfPeopleDetected, this);

    }
    else if(numberOfPeopleDetected > preNumberOfPeopleDetected) {
        ofNotifyEvent(personDetected,numberOfPeopleDetected, this);
    }
    preNumberOfPeopleDetected = numberOfPeopleDetected;
}
//--------------------------------------------------------------
void Detection_Abstraction::draw(int x, int y)
{
    ofPushMatrix();
    ofTranslate(x, y);
    ofSetColor(ofColor::white);
    
    int w = videoPlayer.getWidth();
    int h = videoPlayer.getHeight();
    int offset = 10;
    ofSetColor(ofColor::white);
    if (!frameGray.empty()) {
        ofxCv::drawMat(frameGray, 0, 0,w,h);
    }
    if (!background.empty()) {
        ofxCv::drawMat(background, w+offset, 0,w,h);
    }
    if (!abDifference.empty()) {
        ofxCv::drawMat(abDifference, (w+offset)*2, 0,w,h);
    }
    if (!abDifferenceMutated.empty()) {
        ofxCv::drawMat(abDifferenceMutated, 0, h+offset,w,h);
    }
    if (!dilatedMat.empty()) {
        ofxCv::drawMat(dilatedMat, (w+offset), h+offset,w,h);
    }
    if (!erodedMat.empty()) {
        ofxCv::drawMat(erodedMat, (w+offset)*2, h+offset,w,h);
    }
    if (!mutatedMat.empty()) {
        ofxCv::drawMat(mutatedMat, 0, (h+offset)*2,w,h);
    }
    if (!markers.empty()) {
        ofxCv::drawMat(markers, (w+offset), (h+offset)*2,w,h);
    }
    if (!watershedMat.empty()) {
        ofxCv::drawMat(watershedMat, (w+offset)*2, (h+offset)*2,w,h);
    }
    
    font.drawString("Original Input", 0, offset);
    font.drawString("Background", w+offset, offset);
    font.drawString("Abs Difference", (w+offset)*2, offset);
    font.drawString("Abs Difference Mutated",0, h+(offset*2));
    font.drawString("Dilated",(w+offset), h+(offset*2));
    font.drawString("Eroded",(w+offset)*2, (h+(offset*2)));
    font.drawString("Mutated",0, (h+offset)*2+offset);
    font.drawString("Eroded+Dilated Combine",(w+offset), (h+offset)*2+offset);
    font.drawString("Watershed Segmetation",(w+offset)*2, (h+offset)*2+offset);

    stringstream ss;
    ss << "Mean of Mat: " << meanOfMatrix << endl;
    ss << "Number of Frames: " << frames << " Calibration Frames:" << calibrationLength << endl;

    
    font.drawString(ss.str(),(w+offset)*3, offset);
    ofPushMatrix();

    ofSetColor(ofColor::red);
    ofFill();
    contourFinder.draw();
    ofSetColor(ofColor::blue);
    subFinder.draw();
    ofPopMatrix();
    ofPopMatrix();
    
    
    if (showOutput) {
        ofPushMatrix();
        ofTranslate(w+(offset*2),offset);
        ofFill();
        ofSetColor(ofColor::white);
        ofDrawRectangle(0, 0, w, h);
        int nBlobs = contourFinder.getContours().size();
        for (int i = 0; i < nBlobs; i++) {
            ofBeginShape();
            ofFill();
            ofSetColor(ofColor::black);
            for (int e = 0 ; e < contourFinder.getContours()[i].size(); e++) {
                ofVertex(contourFinder.getContours()[i][e].x, contourFinder.getContours()[i][e].y);
            }
            ofEndShape(true);
        }
        ofPopMatrix();
    }
}
//--------------------------------------------------------------
void Detection_Abstraction::showResult(bool show)
{
    showOutput = show;
}
//--------------------------------------------------------------
void Detection_Abstraction::setAlpha(float _alpha)
{
    alphaBlend = _alpha;
}
//--------------------------------------------------------------
void Detection_Abstraction::setBlur(int _blur)
{
    blurAmount = _blur;
}
//--------------------------------------------------------------
void Detection_Abstraction::setDilateAmount(int _dilation)
{
    dilationAmount = _dilation;
}
//--------------------------------------------------------------
void Detection_Abstraction::setNoiseRemoval(int _noiseRemoval)
{
    noiseRemoval = _noiseRemoval;
}
//--------------------------------------------------------------
void Detection_Abstraction::setErodeAmount(int _erosion)
{
    erosionAmount = _erosion;
}
//--------------------------------------------------------------
void Detection_Abstraction::setMinRadius(int minRadius)
{
    contourFinder.setMinAreaRadius(minRadius);
    subFinder.setMinAreaRadius(minRadius);
}
//--------------------------------------------------------------
void Detection_Abstraction::setMaxRadius(int maxRadius)
{
    contourFinder.setMaxAreaRadius(maxRadius);
    subFinder.setMaxAreaRadius(maxRadius);
}
//--------------------------------------------------------------
void Detection_Abstraction::setFirstThreshold(int _threshold)
{
    firstThresholdValue = _threshold;
}
//--------------------------------------------------------------
void Detection_Abstraction::setSecondThreshold(int _threshold)
{
    secondThresholdValue = _threshold;
}
//--------------------------------------------------------------
void Detection_Abstraction::setThirdThreshold(int _threshold)
{
    thirdThresholdValue = _threshold;
}
//--------------------------------------------------------------
void Detection_Abstraction::setCustomThreshold(int _threshold)
{
    customThresholdValue = _threshold;
}
//--------------------------------------------------------------
void Detection_Abstraction::setContourThreshold(int _contThreshold)
{
    contourFinder.setThreshold(_contThreshold);
    subFinder.setThreshold(_contThreshold);
}
//--------------------------------------------------------------
void Detection_Abstraction::setCalibrationLength(int _length)
{
    calibrationLength = _length;
}
//--------------------------------------------------------------
void Detection_Abstraction::setMorphology(int mode)
{
    morphology = mode;
}
//--------------------------------------------------------------
void Detection_Abstraction::resetCalibration()
{
    frames = 0;
    background = cv::Mat(frameMat.rows,frameMat.cols,CV_8UC1);
    for (int y = 0; y < background.rows; y++) {
        for (int x = 0; x < background.cols; x++) {
            background.at<uchar>(y,x) = 0;
        }
    }
}
//--------------------------------------------------------------
void Detection_Abstraction::useEqualizer(bool enable)
{
    equalize = enable;
}
