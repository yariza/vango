#include "Style.h"
#include <boost/filesystem.hpp>

using namespace cv;

void LayerStyle::loadTextures(std::string yamlPath)
{
    namespace fs = boost::filesystem;
    fs::path yamlDir = (fs::path(yamlPath)).parent_path();

    fs::path texAbsPath = yamlDir / fs::path(texPath);
    fs::path maskAbsPath = yamlDir / fs::path(maskPath);

    texImage = imread(texAbsPath.native(), CV_LOAD_IMAGE_COLOR);
    maskImage = imread(maskAbsPath.native(), CV_LOAD_IMAGE_COLOR);

    assert(texImage.data != NULL);
    assert(maskImage.data != NULL);
}

void CanvasStyle::loadTextures(std::string yamlPath)
{
    for (uint i=0; i<layers.size(); i++) {
        layers[i].loadTextures(yamlPath);
    }
}