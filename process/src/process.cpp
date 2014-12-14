#include <stdio.h>
#include <tclap/CmdLine.h>
#include "Processor.h"

using namespace TCLAP;

std::string imgFile;
std::string styleFile;
std::string outFile;
bool actuallyWrite; 

Processor processor; 

void parseCommandLine(int argc, char** argv){
    try {
        CmdLine cmd("Vango Process", ' ', "0.001");

        SwitchArg outSwitch("d", "don't_write", "don't actually write to file", cmd, false);

        ValueArg<string> outFileName("o", "output", "output yaml file to write the canvas information to", "true", "out.yaml", "string");
        cmd.add(outFileName);

        ValueArg<string> styleFileName("s", "style", "style file defining stylistic parameters", true, "style.yaml", "string");
        cmd.add(styleFileName);

        ValueArg<string> imgFileName("i", "image", "image file to process", true, "image.png", "string");
        cmd.add(imgFileName);

        cmd.parse(argc, argv);

        imgFile = imgFileName.getValue();
        styleFile = styleFileName.getValue();
        outFile = outFileName.getValue();
        actuallyWrite = !outSwitch.getValue();
    } 
    catch (ArgException &e) { 
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; 
    }
}   

int main(int argc, char** argv) {
    parseCommandLine(argc, argv);
    
    processor.initialize(imgFile, styleFile, true);

    processor.processImage();
    
    if(actuallyWrite)
        processor.saveToFile(outFile);    
}
