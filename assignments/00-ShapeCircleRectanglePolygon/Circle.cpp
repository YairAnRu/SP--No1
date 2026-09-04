#include "Circle.hpp"

Circle::Circle()
{
    r=0;
}

Circle::Circle(int valX, int valY, int valR) : Shape{valX, valY}
{
    r=valR;
}

int Circle::getRadio()
{
    return r;
}