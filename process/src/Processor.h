#ifndef __PROCESSOR__
#define __PROCESSOR__

#include <opencv2/opencv.hpp>
#include <stdio.h>
//#include "canvas/Brushstroke.h"

class Processor {
public:
    Processor(std::string imgFile);

    virtual ~Processor();
    
    virtual void display(std::string windowName="window");

private:
    cv::Mat image;

};


#endif
