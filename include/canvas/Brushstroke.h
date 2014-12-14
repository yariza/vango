#ifndef __BRUSHSTROKE_H__
#define __BRUSHSTROKE_H__

#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>
#include "ColorUtils.h"

#define ANCHOR_KEY "anchor"
#define ANGLE_KEY "angle"
#define LENGTH1_KEY "length1"
#define LENGTH2_KEY "length2"
#define WIDTH_KEY "width"
#define OPACITY_KEY "opacity"
#define COLOR_KEY "color"

using namespace cv;

class Brushstroke {

public:
    // anchor point
    Point2d anchor;

    // orientation of stroke in Radians, going CCW, with 0 along positive x-axis
    double angle;

    // inertia of stroke in orientation-determining algorithm
    double strength;

    // lengths of the stroke
    double length1;  // goes in angle direction
    double length2;  // goes in -angle direction

    double width;

    // opacity (0-1)
    double opacity;

    // color (R,G,B). Types are in double (0-1).
    Vec3d color;

    void draw(Mat& alphaMap, Mat& mask,
              double spacing, double jitter);

private:
    void renderTexture(Mat& alphaMap, Mat& mask, RotatedRect& maskRect);
};

namespace YAML {

    template<>
    struct convert<Vec3d> {
        static Node encode(const Vec3d& rhs) {
            Node node;
            node.push_back(rhs[0]);
            node.push_back(rhs[1]);
            node.push_back(rhs[2]);
            return node;
        }

        static bool decode(const Node& node, Vec3d& rhs) {
            if(!node.IsSequence() || node.size() != 3) {
                return false;
            }

            rhs[0] = node[0].as<double>();
            rhs[1] = node[1].as<double>();
            rhs[2] = node[2].as<double>();
            return true;
        }
    };

    template<>
    struct convert<Point2d> {
        static Node encode(const Point2d& rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, Point2d& rhs) {
            if (!node.IsSequence() || node.size() != 2) {
                return false;
            }

            rhs.x = node[0].as<double>();
            rhs.y = node[1].as<double>();
            return true;
        }
    };

    template<>
    struct convert<Brushstroke> {
        static Node encode(const Brushstroke& stroke) {
            Node node;
            node[ANCHOR_KEY] = stroke.anchor;
            node[ANGLE_KEY] = stroke.angle;
            node[LENGTH1_KEY] = stroke.length1;
            node[LENGTH2_KEY] = stroke.length2;
            node[WIDTH_KEY] = stroke.width;
            node[OPACITY_KEY] = stroke.opacity;
            node[COLOR_KEY] = stroke.color;
            return node;
        }

        static bool decode(const Node& node, Brushstroke& stroke) {
            if (!node.IsMap())
                return false;

            stroke.anchor = node[ANCHOR_KEY].as<Point2d>();
            stroke.angle = node[ANGLE_KEY].as<double>();
            stroke.length1 = node[LENGTH1_KEY].as<double>();
            stroke.length2 = node[LENGTH2_KEY].as<double>();
            stroke.width = node[WIDTH_KEY].as<double>();
            stroke.opacity = node[OPACITY_KEY].as<double>();
            stroke.color = node[COLOR_KEY].as<Vec3d>();

            return true;
        }
    };

}

#endif
