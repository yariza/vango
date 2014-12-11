#include "Brushstroke.h"

Brushstroke::Brushstroke()
: anchor()
, angle()
, strength()
, length1()
, length2()
, width()
, opacity()
, color()
, texImage()
, texSpacing()
, texJitter()
{}


Brushstroke::Brushstroke(Mat& image, double spacing, double jitter)
: texImage(&image)
, texSpacing(spacing)
, texJitter(jitter)
{
}