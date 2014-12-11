#include <stdio.h>
#include <boost/filesystem.hpp>
#include <tclap/CmdLine.h>
#include "Renderer.h"

using namespace cv;
using namespace TCLAP;

std::string canvasFile;
std::string styleFile;

Renderer renderer;

void debugLog(Canvas canvas, CanvasStyle style)
{
    style.loadTextures(styleFile);

    std::cout << canvas.width << std::endl;
    std::cout << canvas.height << std::endl;
    for (uint i=0; i<canvas.layers.size(); i++) {
        Layer layer = canvas.layers[i];
        for (uint j=0; j<layer.strokes.size(); j++) {
            Brushstroke stroke = layer.strokes[j];

            std::cout << "Anchor:  " << stroke.anchor << std::endl;
            std::cout << "Angle:   " << stroke.angle << std::endl;
            std::cout << "Length1: " << stroke.length1 << std::endl;
            std::cout << "Length2: " << stroke.length2 << std::endl;
            std::cout << "Width:   " << stroke.width << std::endl;
            std::cout << "Opacity: " << stroke.opacity << std::endl;
            std::cout << "Color:   " << stroke.color << std::endl;
        }
    }

    std::cout << "Canvas scale: " << style.canvasScale << std::endl;
    for (uint i=0; i<style.layers.size(); i++) {
        LayerStyle layerStyle = style.layers[i];
        std::cout << "tex_path: " << layerStyle.texPath << std::endl;
        std::cout << "tex_spacing: " << layerStyle.texSpacing << std::endl;
        // std::cout << layerStyle.texImage << std::endl;
    }
}

void parseCommandLine(int argc, char** argv)
{
    try {

        CmdLine cmd("Vango Render", ' ', "0.001");

        UnlabeledValueArg<std::string> canvasFileName("canvas", "canvas file to process", true, "", "canvas.yaml", "string");
        cmd.add(canvasFileName);

        ValueArg<std::string> styleFileName("s", "style", "style file defining stylistic parameters", true, "style.yaml", "string");
        cmd.add(styleFileName);

        cmd.parse(argc, argv);

        canvasFile = canvasFileName.getValue();
        styleFile = styleFileName.getValue();

        // std::cout << "run process on " << canvasFile << " as determined by " << styleFile << "..." << std::endl;
    }
    catch (ArgException &e) { // catch any exceptions
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(1);
    }
}

void loadCanvas()
{
    YAML::Node canvasNode = YAML::LoadFile(canvasFile);
    Canvas canvas = canvasNode.as<Canvas>();

    YAML::Node styleNode = YAML::LoadFile(styleFile);
    CanvasStyle style = styleNode.as<CanvasStyle>();

    renderer.load(canvas, style);
}

int main(int argc, char** argv )
{
    // test();

    parseCommandLine(argc, argv);

    loadCanvas();

    debugLog(renderer.canvas, renderer.style);

    // if ( argc != 2 ) {
    //     printf("usage: ./render <Image_Path>\n");
    //     return -1;
    // }

    // Mat image;
    // image = imread( argv[1], 1 );

    // if ( !image.data ) {
    //     printf("No image data \n");
    //     return -1;
    // }
    // namedWindow("render output", CV_WINDOW_AUTOSIZE );
    // imshow("render output", image);

    // waitKey(0);

    // return 0;
}
