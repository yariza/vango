#include "Renderer.h"

using namespace cv;

Renderer::Renderer(): canvas(), style()
{}

void Renderer::load(Canvas my_canvas, CanvasStyle my_style, bool my_simpleMode)
{
    canvas = my_canvas;
    style = my_style;
    simpleMode = my_simpleMode;
}

void Renderer::display(bool newWindow, bool wait)
{
    if (newWindow)
        namedWindow("Vango Renderer", CV_WINDOW_NORMAL);

    imshow("Vango Renderer", color);

    if (wait)
        waitKey(0);
}

void Renderer::initialize()
{
    color = Mat::zeros(canvas.height, canvas.width, CV_64FC3);
    height = Mat::zeros(canvas.height, canvas.width, CV_64FC3);

    color.setTo(Vec3d(1,1,1));
    canvas.drawBackground(color, height, style, simpleMode);
}

void Renderer::draw()
{
    canvas.draw(color, height, style, simpleMode);
}

void Renderer::draw(int layerIndex, int brushStrokeIndex, int numBrushStrokes)
{
    canvas.draw(color, height, style, simpleMode,
                layerIndex, brushStrokeIndex, numBrushStrokes);
}

void Renderer::writeToFile(std::string path)
{
    Mat outImage = Mat::zeros(canvas.height, canvas.width, CV_8UC3);
    color.convertTo(outImage, CV_8UC3, 255.);
    imwrite(path, outImage);
}
