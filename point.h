#pragma once

struct Point
{
    double x;
    double y;

    Point(double _x = 0.0, double _y = 0.0)
        : x { _x }
        , y { _y }
    {}
};
