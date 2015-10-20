#ifndef __CANVAS_H__
#define __CANVAS_H__

#include "BackgroundLayer.h"

#define CANVAS_WIDTH_KEY "width"
#define CANVAS_HEIGHT_KEY "height"
#define CANVAS_LAYERS_KEY "layers"

using namespace cv;

class Canvas
{
public:
    int width;
    int height;
    std::vector<Layer> layers;
    BackgroundLayer background;

    void draw(Mat& color, Mat& height, CanvasStyle& style, bool simple);
    void draw(Mat& color, Mat& height, CanvasStyle& style, bool simple, int layerIndex, int brushStrokeIndex, int numBrushStrokes);
    void drawBackground(Mat& color, Mat& height, CanvasStyle& style, bool simple);
};

namespace YAML {

    template<>
    struct convert<Canvas> {
        static Node encode(const Canvas& canvas) {
            Node node;
            node[CANVAS_WIDTH_KEY] = canvas.width;
            node[CANVAS_HEIGHT_KEY] = canvas.height;
            for (uint i=0; i<canvas.layers.size(); i++) {
                if (canvas.layers[i].strokes.size() > 1)
                    node[CANVAS_LAYERS_KEY].push_back(canvas.layers[i]);
            }
            return node;
        }

        static bool decode(const Node& node, Canvas& canvas) {
            if (!node.IsMap())
                return false;

            canvas.width = node[CANVAS_WIDTH_KEY].as<int>();
            canvas.height = node[CANVAS_HEIGHT_KEY].as<int>();
            for (int i=0; i<node[CANVAS_LAYERS_KEY].size(); i++) {
                if (!node[CANVAS_LAYERS_KEY][i].IsSequence()) {
                    Layer l;
                    canvas.layers.push_back(l);
                } else {
                    canvas.layers.push_back(node[CANVAS_LAYERS_KEY][i].as<Layer>());
                }
            }
            return true;
        }
    };
}

#endif
