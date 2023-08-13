#ifndef POINT_H
#define POINT_H

#include <cstddef>

using namespace std;

struct Point
{
    size_t first;
    size_t second;

    Point() : first(0), second(0) {}

    Point(size_t f, size_t s) : first(f), second(s) {}

    bool operator==(const Point& other) const { return first == other.first && second == other.second; }
};

#endif // POINT_H
