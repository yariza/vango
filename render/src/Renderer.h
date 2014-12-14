#include "canvas/Canvas.h"
#include "canvas/Style.h"

class Renderer {
public:
    Renderer();

    void load(Canvas _canvas, CanvasStyle _style);
    void display(bool newWindow, bool wait);
    void initialize();
    void draw(bool simpleMode);
    void writeToFile(std::string path);

    CanvasStyle style;
    Canvas canvas;

private:
    Mat color;
    Mat height;
};
