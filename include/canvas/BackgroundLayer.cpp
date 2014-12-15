#include "BackgroundLayer.h"

void BackgroundLayer::draw(Mat& colorMap, Mat& heightMap, LayerStyle& style, bool simple)
{
    if (simple)
        return;

    Mat fg_c = Mat::zeros(colorMap.rows, colorMap.cols, CV_64FC3);
    Mat fg_a = Mat::zeros(colorMap.rows, colorMap.cols, CV_64FC3);
    Mat fg_h = Mat::zeros(colorMap.rows, colorMap.cols, CV_64FC3);
    fg_a.setTo(1);

    tileTexture(style.texImage, fg_h);
    fg_h.copyTo(fg_c);

    blend(colorMap, heightMap, fg_c, fg_a, fg_h, style.opacity);
}
