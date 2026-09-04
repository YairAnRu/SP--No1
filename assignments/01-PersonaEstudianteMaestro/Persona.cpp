#include "Persona.hpp"
#include <iostream>

Persona::Persona() {
    nombre="N/A";
    edad=0;
}

Persona::Persona(std::string nom, int ed)
{
    nombre = nom;
    edad = ed;
}

std::string Persona::getNombre()
{
    return nombre;
}

void Persona::setNombre(std::string nom)
{
    nombre = nom;
}

int Persona::getEdad()
{
    return edad;
}

void Persona::setEdad(int ed)
{
    edad = ed;
}

void Persona::muestraDatos()
{
    std::cout << "Nombre:" << nombre << " edad: " << edad << "\n";
}
