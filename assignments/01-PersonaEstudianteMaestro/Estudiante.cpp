#include "Estudiante.hpp"
#include <iostream>

Estudiante::Estudiante():Persona {} {
    carrera="N/A";
}

Estudiante::Estudiante(std::string nom, int ed, std::string ca) : Persona {nom,ed} {
    carrera=ca;
}

std::string Estudiante::getCarrera() {
    return carrera;
}

void Estudiante::setCarrera(std::string ca) {
    carrera = ca;
}

void Estudiante::muestraDatos()
{
    std::cout << "Nombre: " << nombre << " Edad: " << edad << " Carrera: " << carrera << "\n";
}