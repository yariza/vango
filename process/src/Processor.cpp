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

void Processor::blurImage(double kernelwidth, double kernelheight){
    Mat blurImg = image.clone();
    //for (int i = 1; i < max_kernel_length; i += 2){
        // Size(w,h): size of the kernel to be used, with w, h odd and positive
        // stdevx, stddevy... if pass in 0, calculated from kernel size
        GaussianBlur(image, blurImg, Size(kernelwidth, kernelheight), 0, 0); 
    //}    
    image = blurImg;

}

void Processor::display(std::string windowName){
    namedWindow(windowName, WINDOW_AUTOSIZE);
    imshow(windowName, image);

    waitKey(0);
    return;
}




