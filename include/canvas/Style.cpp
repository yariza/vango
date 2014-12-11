#include "Style.h"

using namespace cv;

void LayerStyle::loadTextures()
{
    texImage = imread(texPath, CV_LOAD_IMAGE_COLOR);
    maskImage = imread(maskPath, CV_LOAD_IMAGE_COLOR);

    assert(texImage.data != NULL);
    assert(maskImage.data != NULL);
}

void CanvasStyle::loadTextures()
{
    for (uint i=0; i<layers.size(); i++) {
        layers[i].loadTextures();
    }
}