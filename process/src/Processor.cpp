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
    Mat blurimg = image.clone();
    //for (int i = 1; i < max_kernel_length; i += 2){
        // Size(w,h): size of the kernel to be used, with w, h odd and positive
        // stdevx, stddevy... if pass in 0, calculated from kernel size
        GaussianBlur(image, blurimg, Size(kernelwidth, kernelheight), 0, 0); 
    //}    
    image = blurimg;

}

void Processor::doSobel(int kernelsize){
    Mat grayimg;
    cvtColor(image, grayimg, CV_RGB2GRAY);
    Mat grad;
    // Sobel(src, dst, int ddepth, int dx, int dy, int ksize, double scale, double delta, int borderType)
    // ddepth: output image depth, same as source if set to -1
    // xorder: order of derivative x
    // yorder: order of derivative y
    // ksize: size of extended Sobel kernel
    // scale: optional scaling of output
    // delta: optional value to add to results
    // borderType: extrapolation method at borders
    Sobel(grayimg, grad, -1, 1, 1, kernelsize, 1.0, 0, BORDER_DEFAULT);
    convertScaleAbs(grad, grad);

    namedWindow("gradWind", CV_WINDOW_AUTOSIZE);
    imshow("gradWind", grad);
    waitKey(0);


}



void Processor::display(std::string windowName){
    namedWindow(windowName, WINDOW_AUTOSIZE);
    imshow(windowName, image);

    waitKey(0);
    return;
}




