#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "canvas/Brushstroke.h"

using namespace cv;

int main(int argc, char** argv )
{
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