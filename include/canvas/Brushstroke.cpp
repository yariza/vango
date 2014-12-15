#include "Brushstroke.h"
#include <cmath>

using namespace cv;

void Brushstroke::draw(Mat& alphaMap, Mat& mask,
                       double spacing, double jitter)
{
    Point2d normal = Point2d(cos(angle), sin(angle));

    Point2d point1 = normal * length1;
    Point2d point2 = normal * -length2;

    point1 += anchor;
    point2 += anchor;

    Point2d texCenter;
    RotatedRect maskRect;
    double texAngle;
    Size_<double> maskSize = mask.size();
    double ratio = width / maskSize.width;

    Mat scaledMask;
    resize(mask, scaledMask, Size(), ratio, ratio, INTER_CUBIC);
    maskSize = scaledMask.size();

    RNG rng(0xFFFFFFFF);

    for(double dist = -length2; dist < length1; dist += spacing) {
        texCenter = anchor + normal * dist;
        texAngle = angle + jitter * rng.uniform(-1.0, 1.0);

        maskRect = RotatedRect(texCenter, maskSize, texAngle);
        Rect2d boundingRect = maskRect.boundingRect();

        renderTexture(alphaMap, scaledMask, maskRect);
    }
}

void Brushstroke::simpleDraw(Mat& colorMap, Mat& alphaMap)
{
    Point2d normal = Point2d(cos(angle), sin(angle));

    Point2d point1 = normal * length1;
    Point2d point2 = normal * -length2;

    point1 += anchor;
    point2 += anchor;

    line(colorMap, point1, point2, RGB_TO_SCALAR(color), (int)width, CV_AA);
    line(alphaMap, point1, point2, CV_RGB(1,1,1), (int)width, CV_AA);
}

void Brushstroke::renderTexture(Mat& alphaMap, Mat& mask, RotatedRect& maskRect)
{
    // transformations, etc

    Rect2d boundingRect = maskRect.boundingRect();

    int offsetX = (boundingRect.width - mask.cols)/2;
    int offsetY = (boundingRect.height - mask.rows)/2;

    Mat paddedMask = Mat::zeros(boundingRect.height, boundingRect.width, CV_64FC3);
    mask.copyTo(paddedMask(Rect(offsetX, offsetY, mask.cols, mask.rows)));

    Mat rot_mat = getRotationMatrix2D(Point2f(boundingRect.width/2, boundingRect.height/2),
                                      maskRect.angle / PI * 180.0, 1.0);
    warpAffine(paddedMask, paddedMask, rot_mat, paddedMask.size());

    int x1 = boundingRect.x;
    int y1 = boundingRect.y;
    int x2 = x1 + boundingRect.width;
    int y2 = y1 + boundingRect.height;

    x1 = CLAMP(x1, 0, alphaMap.cols);
    y1 = CLAMP(y1, 0, alphaMap.rows);
    x2 = CLAMP(x2, 0, alphaMap.cols);
    y2 = CLAMP(y2, 0, alphaMap.rows);

    int wid = x2-x1;
    int hig = y2-y1;

    Rect src_roi(x1 - boundingRect.x, y1 - boundingRect.y,
                                 wid, hig);
    Rect dst_roi(x1, y1, wid, hig);

    Mat one_minus_src = Mat::ones(hig, wid, CV_64FC3);
    one_minus_src.setTo(1);
    one_minus_src -= paddedMask(src_roi);

    alphaMap(dst_roi) = paddedMask(src_roi) + alphaMap(dst_roi).mul(one_minus_src);
}
