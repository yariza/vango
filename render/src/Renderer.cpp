#include "Renderer.h"

using namespace cv;

Renderer::Renderer(): canvas(), style()
{}

void Renderer::load(Canvas my_canvas, CanvasStyle my_style)
{
    canvas = my_canvas;
    style = my_style;
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
}

void Renderer::draw()
{
    canvas.draw(color, height, style);
}

void Renderer::writeToFile(std::string path)
{
    color *= 255.;
    Mat outImage = Mat::zeros(canvas.height, canvas.width, CV_8UC3);
    color.convertTo(outImage, CV_8UC3);
    imwrite(path, outImage);
}
