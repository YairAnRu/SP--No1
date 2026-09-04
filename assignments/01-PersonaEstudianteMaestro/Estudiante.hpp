#pragma once

#include "Persona.hpp"
#include <string>

class Estudiante : public Persona {
public:
    Estudiante();
    Estudiante(std::string, int, std::string);

    std::string getCarrera();
    void setCarrera(std::string);

    void muestraDatos();

private:
    std::string carrera;
};