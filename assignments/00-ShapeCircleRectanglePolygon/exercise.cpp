#include "Shape.hpp"
#include "Circle.hpp"

#include <iostream>

int main() 
{
  Shape figura1{1,2};

  std::cout << figura1.draw() << " con valor en x: " << figura1.getValueX() << " y valor en y en: " << figura1.getValueY() << "\n";

  Circle circulo1{2,3,5};

  std::cout << circulo1.draw() << " con valor en x: " << circulo1.getValueX() << " y valor en y en: " << circulo1.getValueY() << " con radio r: "<< circulo1.getRadio() << "\n";

  return 0;
}
