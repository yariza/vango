#include "Style.h"
#include <boost/filesystem.hpp>

using namespace cv;

bool LayerStyle::loadTextures(std::string yamlPath)
{
    namespace fs = boost::filesystem;
    fs::path yamlDir = (fs::path(yamlPath)).parent_path();

    fs::path texAbsPath = yamlDir / fs::path(texPath);
    fs::path maskAbsPath = yamlDir / fs::path(maskPath);

    texImage = imread(texAbsPath.native(), CV_LOAD_IMAGE_COLOR);
    maskImage = imread(maskAbsPath.native(), CV_LOAD_IMAGE_COLOR);

    if (texImage.data == NULL) {
        std::cerr << "error: unable to load texture " << texAbsPath.native() << std::endl;
        return false;
    }
    if (maskImage.data == NULL) {
        std::cerr << "error: unable to load texture " << maskAbsPath.native() << std::endl;
        return false;
    }
    return true;
}

bool CanvasStyle::loadTextures(std::string yamlPath)
{
    for (uint i=0; i<layers.size(); i++) {
        if (!layers[i].loadTextures(yamlPath))
            return false;
    }
    return true;
}
