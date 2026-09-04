#pragma once

#include <string>

class Persona {
public:
    Persona();
    Persona(std::string, int);

    std::string getNombre();
    void setNombre(std::string);

    int getEdad();
    void setEdad(int);

    void muestraDatos();

protected:
    std::string nombre;
    int edad;  
};