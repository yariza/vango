#include "canvas/Canvas.h"
#include "canvas/Style.h"

class Renderer {
public:
    Renderer();

    void load(Canvas _canvas, CanvasStyle _style, bool _simpleMode);
    void display(bool newWindow, bool wait);
    void initialize();
    void draw();
    void draw(int layerIndex, int brushStrokeIndex, int numBrushStrokes);
    void writeToFile(std::string path);

    CanvasStyle style;
    Canvas canvas;
    bool simpleMode;

private:
    Mat color;
    Mat height;
};
