#include <stdio.h>
#include <boost/filesystem.hpp>
#include <tclap/CmdLine.h>
#include "Renderer.h"

using namespace cv;
using namespace TCLAP;

std::string canvasFile;
std::string styleFile;
std::string outputFile;
bool autoOutput;
bool displayEnabled;
bool simpleMode;

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

        ValueArg<std::string> outputFileName("o", "output", "output file of the render", false, "", "string");
        cmd.add(outputFileName);

        SwitchArg autoOutputSwitch("O", "input-name", "write file named like the input file");
        cmd.add(autoOutputSwitch);

        ValueArg<bool> displayArg("d", "display", "run with display enabled if 1, without if 0", false, true, "boolean");
        cmd.add(displayArg);

        SwitchArg simpleArg("p", "simple-paint", "enable simple mode (runs faster)");
        cmd.add(simpleArg);

        cmd.parse(argc, argv);

        canvasFile = canvasFileName.getValue();
        styleFile = styleFileName.getValue();
        outputFile = outputFileName.getValue();
        autoOutput = autoOutputSwitch.getValue();
        displayEnabled = displayArg.getValue();
        simpleMode = simpleArg.getValue();

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

    if (!style.loadTextures(styleFile))
        exit(1);

    renderer.load(canvas, style);
}

int main(int argc, char** argv )
{
    parseCommandLine(argc, argv);

    std::string writeFile = outputFile;

    namespace fs = boost::filesystem;
    if (autoOutput) {
        fs::path writePath = fs::current_path();
        fs::path canvasPath = fs::path(canvasFile);
        writePath /= canvasPath.filename();
        writePath.replace_extension(fs::path("png"));

        writeFile = writePath.native();

        std::cout << writeFile << std::endl;
    }

    loadCanvas();

    // debugLog(renderer.canvas, renderer.style);

    renderer.initialize();
    renderer.draw(simpleMode);

    if (displayEnabled) {
        renderer.display(true, true);
    }

    if (writeFile != "") {
        renderer.writeToFile(writeFile);
        std::cout << "Wrote output to " << writeFile << std::endl;
    }
}
