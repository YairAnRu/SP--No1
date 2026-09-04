#pragma once

#include <string>
class Shape
{
private:
    int x;
    int y;
public:
    Shape();
    Shape(int, int);
    std::string draw();

    int getValueX();
    int getValueY();
};