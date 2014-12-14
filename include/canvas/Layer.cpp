#include "Layer.h"

void Layer::draw(Mat& colorMap, Mat& heightMap, LayerStyle& style)
{
    Mat& mask = style.maskImage;
    double spacing = style.texSpacing;
    double jitter = style.texJitter;
    double layerOpacity = style.opacity;

    Mat fg_c = Mat::zeros(colorMap.rows, colorMap.cols, CV_64FC3);
    Mat fg_a = Mat::zeros(colorMap.rows, colorMap.cols, CV_64FC3);
    Mat fg_h = Mat::zeros(colorMap.rows, colorMap.cols, CV_64FC3);

    // Mat color = Mat::zeros(colorMap.rows, colorMap.cols, CV_64FC3);
    tileTexture(style.texImage, fg_h);

    for (uint i=0; i<strokes.size(); i++) {

        std::cout << "\r  [" << (i+1) << " / " << strokes.size() << "] strokes...   ";

        Brushstroke& stroke = strokes[i];

        fg_c.setTo(Vec3d(stroke.color[2], stroke.color[1], stroke.color[0]));
        fg_a.setTo(0); // clear alpha map

        stroke.draw(fg_a, mask, spacing, jitter);

        fg_a = fg_a.mul(fg_h);

        blend(colorMap, heightMap, fg_c, fg_a, fg_h, layerOpacity*stroke.opacity);
    }
    std::cout << "done." << std::endl;
}

void Layer::blend(Mat& colorMap, Mat& heightMap,
                  Mat& fg_c, Mat& fg_a, Mat& fg_h,
                  double opacity)
{
    fg_a *= opacity;

    Mat fg_1_minus_a = Mat::ones(colorMap.rows, colorMap.cols, CV_64FC3);
    fg_1_minus_a.setTo(1);
    fg_1_minus_a -= fg_a;

    colorMap = colorMap.mul(fg_1_minus_a) + fg_c.mul(fg_a);
    heightMap = heightMap.mul(fg_1_minus_a) + fg_h.mul(fg_a);
}

void Layer::tileTexture(Mat& src, Mat& dst)
{
    for (int x = 0; x < dst.cols; x += src.cols) {
        for(int y = 0; y < dst.rows; y += src.rows) {

            int x2 = x + src.cols;
            int y2 = y + src.rows;

            x2 = CLAMP(x2, 0, dst.cols);
            y2 = CLAMP(y2, 0, dst.rows);

            int wid = x2-x;
            int hig = y2-y;

            Rect src_roi(0, 0, wid, hig);
            Rect dst_roi(x, y, wid, hig);

            src(src_roi).copyTo(dst(dst_roi));
        }
    }
}
