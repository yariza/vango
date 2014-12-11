#include <opencv2/opencv.hpp>

using namespace cv;

class Brushstroke {

public:
    // anchor point
    Point2d anchor;
   
    // orientation of stroke in Radians, going CCW, with 0 along positive x-axis
    double angle;

    // inertia of stroke in orientation-determining algorithm
    double strength; 

    // lengths of the stroke
    double length1;  // goes in angle direction
    double length2;  // goes in -angle direction

    double width;

    // opacity (0-1)
    double opacity;

    // color (R,G,B). Types are in double (0-1).
    Vec3d color;

    // texture image
    Mat texImage;   
    double texSpacing; 
    double texJitter; 
};
