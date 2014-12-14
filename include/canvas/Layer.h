#ifndef __LAYER_H__
#define __LAYER_H__

#include "Brushstroke.h"
#include "Style.h"

using namespace cv;

class Layer {

public:
    std::vector<Brushstroke> strokes;

    void draw(Mat& color, Mat& height, LayerStyle& style, bool simple);
    void blend(Mat& colorMap, Mat& heightMap,
                      Mat& fg_c, Mat& fg_a, Mat& fg_h,
                      double opacity);
    void tileTexture(Mat& src, Mat& dst);
};

namespace YAML {

    template<>
    struct convert<Layer> {
        static Node encode(const Layer& layer) {
            Node node;
            for (uint i=0; i<layer.strokes.size(); i++) {
                node.push_back(layer.strokes[i]);
            }
            return node;
        }

        static bool decode(const Node& node, Layer& layer) {
            if (!node.IsSequence())
                return false;

            for (int i=0; i<node.size(); i++) {
                layer.strokes.push_back(node[i].as<Brushstroke>());
            }
            return true;
        }
    };
}

#endif
