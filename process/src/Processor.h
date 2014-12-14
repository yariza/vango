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
    Processor(std::string imgFile);

    virtual ~Processor();

    virtual void blurImage(double kernelwidth = 11, double kernelheight = 11);

    virtual void doSobel(int kernelsize = 3);
    
    virtual void display(std::string windowName="window");

    virtual void placeStrokes();

private:
    cv::Mat image;

};


#endif
