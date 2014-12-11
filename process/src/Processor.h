#ifndef __PROCESSOR__
#define __PROCESSOR__

#include <opencv2/opencv.hpp>
#include <stdio.h>
//#include "canvas/Brushstroke.h"

class Processor {
public:
    Processor(std::string imgFile);

    virtual ~Processor();

    virtual void blurImage(double kernelwidth = 11, double kernelheight = 11);

    virtual void doSobel(int kernelsize = 3);
    
    virtual void display(std::string windowName="window");

private:
    cv::Mat image;

};


#endif
