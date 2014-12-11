#include <string>
#include <iostream>
#include <algorithm>
#include <tclap/CmdLine.h>
#include "canvas/Brushstroke.h"
#include "Processor.h"

using namespace TCLAP;
using namespace std;

int main(int argc, char** argv)
{
  // Wrap everything in a try block.  Do this every time,
  // because exceptions will be thrown for problems.
  try {

    // Define the command line object.
    CmdLine cmd("Vango Process", ' ', "0.001");

    ValueArg<string> styleFileName("s", "style", "style file defining stylistic parameters", true, "style.yaml", "string");
    cmd.add(styleFileName);

    ValueArg<string> imgFileName("i", "image", "image file to process", true, "image.png", "string");
    cmd.add(imgFileName);

    cmd.parse(argc, argv);

    string imgName = imgFileName.getValue();
    string styleName = styleFileName.getValue();

    std::cout << "run process on " << imgName << " as determined by " << styleName << "..." << std::endl;

    Processor pro(imgName);
    //pro.display();

    //pro.doSobel();

    //pro.blurImage();
    //pro.display();
    //pro.doSobel();

    pro.placeStrokes();    

} catch (ArgException &e)  // catch any exceptions
    { cerr << "error: " << e.error() << " for arg " << e.argId() << endl; }
}
