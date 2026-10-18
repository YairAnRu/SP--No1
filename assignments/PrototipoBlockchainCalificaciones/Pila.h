#ifndef PILA_H
#define PILA_H

#include <iostream>
#include <cstdlib>
#include "DArray.h"

template <typename T>

class TPila{
private:
    DArray<T> datos;

public:
    Pila() {}

    void apilar(T elemento){
        datos.agregar(elemento);
    }

    void desapilar(){
        if (estaVacia()){
            std::cerr << "Error: Pila vacia, no es posible desapilar. \n";
            return;
        }
    datos.eliminarUltimo();
    }

    T tope() const{
        if (estaVacia()){
            std::cerr << "Error: Pila vacia, no hay elemento en el tope. \n";
            std::exit(1);
        }
        return datos.obtenerEn(datos.size() - 1);
    }

    bool estaVacia() const{
        return datos.size() == 0
    }

    int tamanoActual() const{
        return datos.size();
    }

};

#endif