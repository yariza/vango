#include "Processor.h"

using namespace cv;

Processor::Processor(std::string imgName){
    image = imread(imgName, CV_LOAD_IMAGE_COLOR);

    if(!image.data){
        std::cout << "No image data :(" << std::endl;
        return;
    }
}

Processor::~Processor()
{}

void Processor::display(std::string windowName){
    namedWindow(windowName, WINDOW_AUTOSIZE);
    imshow(windowName, image);

    waitKey(0);
    return;
}




