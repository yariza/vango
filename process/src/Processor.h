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

    void buildStrokes(Layer& layer, LayerStyle& lstyle);
    void angleStrokes(Layer& layer, LayerStyle& lstyle);
    void clipStrokes(Layer& layer, LayerStyle& lstyle);
    void colorStrokes(Layer& layer, LayerStyle& lstyle);

    cv::Mat image;
    Canvas canvas;
    CanvasStyle canvStyle; 
};


#endif
