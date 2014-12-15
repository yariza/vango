#include "Style.h"
#include <boost/filesystem.hpp>
#include <climits>

using namespace cv;

bool LayerStyle::loadTextures(std::string yamlPath)
{
    namespace fs = boost::filesystem;
    fs::path yamlDir = (fs::path(yamlPath)).parent_path();

    fs::path texAbsPath = yamlDir / fs::path(texPath);
    fs::path maskAbsPath = yamlDir / fs::path(maskPath);

    texImage = imread(texAbsPath.native(), CV_LOAD_IMAGE_COLOR);

    texImage.convertTo(texImage, CV_64FC3);
    texImage /= UCHAR_MAX;

    {
        Mat one = Mat::ones(texImage.rows, texImage.cols, CV_64FC1);
        Mat t[] = {one, one, one};
        merge(t, 3, one);

        texImage = one - texImage;
    }

    Mat maskImageSource = imread(maskAbsPath.native(), CV_LOAD_IMAGE_UNCHANGED);
    maskImage.create(maskImageSource.rows, maskImageSource.cols, maskImageSource.type());

    Mat channels[4];
    split(maskImageSource, channels);

    Mat A = channels[3];
    Mat R = channels[0];

    bool usingAlpha = (countNonZero(A) > 0);

    if (usingAlpha) {
        Mat from[] = {A, A, A};
        merge(from, 3, maskImage);
    }
    else {
        Mat from[] = {R, R, R};
        merge(from, 3, maskImage);
    }

    maskImage.convertTo(maskImage, CV_64FC3);
    maskImage /= UCHAR_MAX;

    if (!usingAlpha) {
        Mat one = Mat::ones(maskImage.rows, maskImage.cols, CV_64FC1);
        Mat t[] = {one, one, one};
        merge(t, 3, one);

        maskImage = one - maskImage;
    }

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

bool BackgroundLayerStyle::loadTextures(std::string yamlPath)
{
    namespace fs = boost::filesystem;
    fs::path yamlDir = (fs::path(yamlPath)).parent_path();

    fs::path bgAbsPath = yamlDir / fs::path(texPath);

    texImage = imread(bgAbsPath.native(), CV_LOAD_IMAGE_COLOR);

    texImage.convertTo(texImage, CV_64FC3);
    texImage /= UCHAR_MAX;

    if (texImage.data == NULL) {
        std::cerr << "error: unable to load texture " << bgAbsPath.native() << std::endl;
        return false;
    }
    return true;
}

bool CanvasStyle::loadTextures(std::string yamlPath)
{
    if (!bgStyle.loadTextures(yamlPath))
        return false;

    for (uint i=0; i<layers.size(); i++) {
        if (!layers[i].loadTextures(yamlPath))
            return false;
    }
    return true;
}
