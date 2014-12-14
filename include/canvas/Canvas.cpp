#include "Canvas.h"

void Canvas::draw(Mat& color, Mat& height, CanvasStyle& style, bool simple)
{
    for (uint i=0; i<layers.size(); i++) {
        std::cout << "Drawing Layer " << i+1 << "/" << layers.size() << "..." << std::endl;
        Layer& layer = layers[i];
        LayerStyle& layerStyle = style.layers[i];
        layer.draw(color, height, layerStyle, simple);
    }
}
