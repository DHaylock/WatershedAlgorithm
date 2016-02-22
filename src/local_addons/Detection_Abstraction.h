//
//  Detection_Abstraction.h
//  Watershed Algorithm
//
//  Created by David Haylock on 01/02/2016.
//
//

#ifndef Detection_Abstraction_h
#define Detection_Abstraction_h

#include <stdio.h>
#include "ofxCv.h"
#include "ofMain.h"


class WatershedSegmenter {
public:
    void setMarkers(cv::Mat &markerImage) {
        markerImage.convertTo(markers,CV_32S);
    }
    cv::Mat process(cv::Mat &image) {
        cv::watershed(image, markers);
        markers.convertTo(markers,CV_8U);
        return markers;
    }
private:
    cv::Mat markers;
};

class Detection_Abstraction : public ofThread {
    public:
        void setup(int width,int height);
        void update();
        void draw(int x,int y);
    
        WatershedSegmenter segmenter;
    
        //! Restarts the Calibration Loop
        void resetCalibration();

        // Setters
        //! Sets the Alpha Blend
        void setAlpha(float _alpha = 0.01);
    
        //! Sets the Blur
        void setBlur(int _blur);

        //! Sets the Dilation Amount
        void setDilateAmount(int _dilation);
    
        //! Sets the Erosion Amount
        void setErodeAmount(int _erosion);
    
        //! Sets the Min Radius of tracked object
        void setMinRadius(int minRadius);
    
        //! Sets the Max Radius of tracked object
        void setMaxRadius(int maxRadius);
    
        //! Sets the Thresholding Values
        void setFirstThreshold(int _threshold);
        void setSecondThreshold(int _threshold);
        void setThirdThreshold(int _threshold);

        //! Sets the Custom Thresholding Values
        void setCustomThreshold(int _threshold);
    
        //! Sets the amount of Gaussian Blur which removes the camera noise
        void setNoiseRemoval(int _noiseRemoval);
    
        //! Sets Contour Threshold
        void setContourThreshold(int _contThreshold);
    
        //! Set the Calibration length
        void setCalibrationLength(int _length);
    
        //! Sets the current video
        void setVideo(string filePath);
    
        //! Set the Morphology
        void setMorphology(int mode);
    
        //! Equalizes the historgram: Makes the image more contrasty
        void useEqualizer(bool enable);
    
        void showResult(bool show);
    
        // Listener Events
        ofEvent<int> personDetected;
        ofEvent<int> personLeft;
    
    private:
        ofTrueTypeFont font;
    
        ofVideoPlayer videoPlayer;
        cv::Mat frameMat;
        cv::Mat frameGray;
        cv::Mat thresholdedMat;
        cv::Mat accumilator;
        cv::Mat background;
        cv::Mat deNoise;
        cv::Mat smallMat;
        cv::Mat abDifference;
        cv::Mat abDifferenceMutated;
        cv::Mat markers;
        cv::Mat mutatedMat;
        cv::Mat erodedMat;
        cv::Mat dilatedMat;
        cv::Mat tempMarkers;
        cv::Mat watershedMat;
        cv::BackgroundSubtractorMOG2 *bgs;
        cv::Mat bgmodel;
        cv::Mat dist,dist_8u;
        ofxCv::ContourFinder contourFinder;
        ofxCv::ContourFinder subFinder;

        ofxCv::FlowFarneback fb;
        ofxCv::FlowPyrLK lk;
        
        ofxCv::Flow* curFlow;
    
    
        int blurAmount;
        int numberOfPeopleDetected;
        int preNumberOfPeopleDetected;
        int firstThresholdValue;
        int secondThresholdValue;
        int thirdThresholdValue;
        int customThresholdValue;
        int dilationAmount;
        int noiseRemoval;
        int erosionAmount;
        int dilateValue;
        int calibrationLength;
        int frames;
        int meanOfMatrix;
        bool showLabels;
        bool showOutput;
        bool equalize;
        float alphaBlend;
        int morphology;
protected:
    
};

#endif /* Detection_Abstraction_h */
