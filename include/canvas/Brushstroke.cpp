#include "Brushstroke.h"
#include "ColorUtils.h"
#include <cmath>

using namespace cv;

void Brushstroke::draw(Mat& colorMap, Mat& heightMap,
                       Mat& texture, Mat& mask,
                       double layerOpacity, double spacing, double jitter)
{
    Point2d point1 = Point2d(cos(angle), sin(angle)) * length1;
    Point2d point2 = Point2d(cos(angle), sin(angle)) * -length2;

    point1 += anchor;
    point2 += anchor;

    line(colorMap, point1, point2, RGB_TO_SCALAR(color), (int)width, CV_AA);
    std::cout << "line drawn from " << point1 << " to " << point2 << " with color " << color << std::endl;
}
