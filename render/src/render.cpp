#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "canvas/Canvas.h"
#include "canvas/Style.h"

using namespace cv;

int main(int argc, char** argv )
{
    YAML::Node node = YAML::Load(
        "{width: 20, height: 40, layers: [[{anchor: [1.4, 2.6], angle: 0.3, length1: 3, length2: 4, width: 1.43, opacity: 0.3, color: [0.3, 0.4, 0.6]}]] }");
    Canvas canvas = node.as<Canvas>();

    std::cout << canvas.width << std::endl;
    std::cout << canvas.height << std::endl;
    for (uint i=0; i<canvas.layers.size(); i++) {
        Layer layer = canvas.layers[i];
        for (uint j=0; j<layer.strokes.size(); j++) {
            Brushstroke stroke = layer.strokes[j];

            std::cout << "Anchor:  " << stroke.anchor << std::endl;
            std::cout << "Angle:   " << stroke.angle << std::endl;
            std::cout << "Length1: " << stroke.length1 << std::endl;
            std::cout << "Length2: " << stroke.length2 << std::endl;
            std::cout << "Width:   " << stroke.width << std::endl;
            std::cout << "Opacity: " << stroke.opacity << std::endl;
            std::cout << "Color:   " << stroke.color << std::endl;
        }
    }

    if ( argc != 2 ) {
        printf("usage: ./render <Image_Path>\n");
        return -1;
    }

    Mat image;
    image = imread( argv[1], 1 );

    if ( !image.data ) {
        printf("No image data \n");
        return -1;
    }
    namedWindow("render output", CV_WINDOW_AUTOSIZE );
    imshow("render output", image);

    waitKey(0);

    return 0;
}
