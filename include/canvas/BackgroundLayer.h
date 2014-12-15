#ifndef __BACKGROUND_LAYER_H__
#define __BACKGROUND_LAYER_H__

#include "Layer.h"

class BackgroundLayer : public Layer {

public:
    virtual void draw(Mat& color, Mat& height, LayerStyle& style, bool simple);

};

#endif