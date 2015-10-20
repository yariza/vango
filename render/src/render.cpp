#include <stdio.h>
#include <boost/filesystem.hpp>
#include <tclap/CmdLine.h>
#include "Renderer.h"

using namespace cv;
using namespace TCLAP;

// Input canvas filename
std::string canvasFile;

// Input style filename
std::string styleFile;

// Output PNG filename
std::string outputFile;

// Auto-generate output filename based on canvas path
bool autoOutput;

// Is displaying output on screen enabled?
bool displayEnabled;

// Is Simple mode enabled? In this mode, rendering textures is disabled, and
// instead lines are drawn with full opacity.
bool simpleMode;

int videoFrames;

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

        ValueArg<int> videoFramesArg("v", "video-frames", "frames for video output", false, 0, "int");
        cmd.add(videoFramesArg);

        cmd.parse(argc, argv);

        canvasFile = canvasFileName.getValue();
        styleFile = styleFileName.getValue();
        outputFile = outputFileName.getValue();
        autoOutput = autoOutputSwitch.getValue();
        displayEnabled = displayArg.getValue();
        simpleMode = simpleArg.getValue();
        videoFrames = videoFramesArg.getValue();

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

    renderer.load(canvas, style, simpleMode);
}

std::string filePathForName(std::string outputName)
{
    if (outputName == "" && !autoOutput)
        return "";

    namespace fs = boost::filesystem;

    std::string writeFile = outputName;
    std::string directoryName = fs::path(outputName).stem().native();

    fs::path canvasPath = fs::path(canvasFile);
    std::string canvasName = canvasPath.stem().native();

    if (autoOutput) {
        directoryName = canvasName;
    }

    fs::path writePath = fs::path(outputName).parent_path();
    if (autoOutput) {
        writePath = fs::current_path();
    }
    writePath /= directoryName;

    // make this directory if it doesn't exist already
    create_directory(writePath);

    writePath /= (canvasName + ".png");

    writeFile = writePath.native();

    return writeFile;
}

std::string filePathForName(std::string outputName, int frameNumber)
{
    if (outputName == "" && !autoOutput)
        return "";

    namespace fs = boost::filesystem;

    std::string writeFile = outputName;
    std::string directoryName = fs::path(outputName).stem().native();

    fs::path canvasPath = fs::path(canvasFile);
    std::string canvasName = canvasPath.stem().native();

    if (autoOutput) {
        directoryName = canvasName;
    }

    fs::path writePath = fs::path(outputName).parent_path();
    if (autoOutput) {
        writePath = fs::current_path();
    }
    writePath /= directoryName;

    // make this directory if it doesn't exist already
    create_directory(writePath);

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(5) << frameNumber;
    writePath /= (canvasName + "_" + oss.str());

    writePath.replace_extension(fs::path("png"));
    writeFile = writePath.native();

    return writeFile;
}

int main(int argc, char** argv )
{
    parseCommandLine(argc, argv);

    loadCanvas();

    // debugLog(renderer.canvas, renderer.style);

    renderer.initialize();

    if (videoFrames > 1) {

        int totalStrokes = 0;
        for (int i=0; i<renderer.canvas.layers.size(); i++) {
            Layer& layer = renderer.canvas.layers[i];
            totalStrokes += layer.strokes.size();
        }
        int currentLayerIndex = 0;
        int currentLayerStrokeIndex = 0;
        int strokeCount = 0;
        int currentFrame = 0;
        while (strokeCount < totalStrokes) {
            std::cout << "rendering frame " << currentFrame << "..." << std::endl;
            int newStrokeCount = totalStrokes * currentFrame / videoFrames;
            while (strokeCount < newStrokeCount) {
                Layer& currentLayer = renderer.canvas.layers[currentLayerIndex];
                // check if currentLayerStrokeIndex has reached capacity
                // if so, increment layer and reset value
                if (currentLayer.strokes.size() <= currentLayerStrokeIndex) {
                    currentLayerIndex++;
                    currentLayerStrokeIndex = 0;
                }
                int maxStrokes = newStrokeCount - strokeCount;
                int strokesForLayer = currentLayer.strokes.size() - currentLayerStrokeIndex;
                strokesForLayer = (strokesForLayer < maxStrokes) ? strokesForLayer : maxStrokes;

                std::cout << ">Drawing Layer " << currentLayerIndex
                          << " - strokes " << currentLayerStrokeIndex
                          << " to " << (currentLayerStrokeIndex + strokesForLayer) << std::endl;
                renderer.draw(currentLayerIndex, currentLayerStrokeIndex, strokesForLayer);

                // update values
                strokeCount += strokesForLayer;
                currentLayerStrokeIndex += strokesForLayer;
            }

            if (displayEnabled) {
                renderer.display(true, false);
            }
            std::string filePath = filePathForName(outputFile, currentFrame);
            if (filePath != "") {
                renderer.writeToFile(filePath);
                std::cout << "Wrote output to " << filePath << std::endl;
            }
            currentFrame++;
        }
    }
    else {
        renderer.draw();
        if (displayEnabled) {
            renderer.display(true, true);
        }
        std::string filePath = filePathForName(outputFile);
        if (filePath != "") {
            renderer.writeToFile(filePath);
            std::cout << "Wrote output to " << filePath << std::endl;
        }
    }
}
