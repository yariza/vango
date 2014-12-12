#include "Canvas.h"

void Canvas::draw(Mat& color, Mat& height, CanvasStyle& style)
{
    for (uint i=0; i<layers.size(); i++) {
        Layer& layer = layers[i];
        LayerStyle& layerStyle = style.layers[i];
        layer.draw(color, height, layerStyle);
    }
}
