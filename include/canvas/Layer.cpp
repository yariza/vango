#include "Layer.h"

void Layer::draw(Mat& color, Mat& height, LayerStyle& style)
{
    Mat& texture = style.texImage;
    Mat& mask = style.maskImage;
    double spacing = style.texSpacing;
    double jitter = style.texJitter;
    double layerOpacity = style.opacity;

    for (uint i=0; i<strokes.size(); i++) {
        Brushstroke& stroke = strokes[i];
        stroke.draw(color, height, texture, mask, layerOpacity, spacing, jitter);
    }
}
