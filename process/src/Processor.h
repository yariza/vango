#ifndef __PROCESSOR__
#define __PROCESSOR__

#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include "canvas/Brushstroke.h"
#include "canvas/Canvas.h"
#include "canvas/Layer.h"
#include "canvas/ColorUtils.h"

class Processor {
public:
    
    void initialize(std::string imgFile, std::string styleFile); 
    void initialize(std::string imgFile, std::string styleFile, bool verby);
    void processImage();
    void saveToFile(std::string outFile);    




    void ignorethisblurImage(double kernelwidth = 11, double kernelheight = 11);

    void ignorethisdoSobel(int kernelsize = 3);
    
    void ignorethisdisplay(std::string windowName="window");

    void ignorethisplaceStrokes();

    bool verbose;     

private:

    void buildStrokes(Layer& layer, LayerStyle& lstyle, int lid);
    void angleStrokes(Layer& layer, LayerStyle& lstyle, int lid);
    void clipStrokes(Layer& layer, LayerStyle& lstyle, int lid);
    void colorStrokes(Layer& layer, LayerStyle& lstyle, int lid);
    
    void makeDummyStroke(Brushstroke& stroke, cv::Point2d ankh, double avgWb, double dWb, double opac);
    void createRegenMask(cv::Mat& mask, int lid, double rmaskwidth);
    double distCanvas(cv::Point2d& x1, cv::Point2d& x2);
    void interpAngles(Layer& layer, LayerStyle& lstyle, int lid);
    void displayImage(cv::Mat& img, std::string windowName);

    cv::Mat image;
    std::vector<cv::Mat> blurimages; 
    std::vector<cv::Mat> gradXimages;
    std::vector<cv::Mat> gradYimages;
    std::vector<cv::Mat> gradimages;
    Canvas canvas;
    CanvasStyle canvStyle; 
};


#endif
