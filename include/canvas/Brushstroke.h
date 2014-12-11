#include <opencv2/opencv.hpp>

using namespace cv;

class Brushstroke {

public:
    // anchor point
    Point2d anchor;

    // lengths of the stroke
    double length1;
    double length2;

    double width;

    // opacity (0-1)
    double opacity;

    // color (R,G,B). Types are in double (0-1).
    Vec3d color;
};