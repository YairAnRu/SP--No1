#ifndef COLA_H
#define COLA_H

#include "DArray.h"
#include <iostream>
#include <cstdlib>

template <typename T>

class Cola{
private:
    DArray<T> datos;
    int frente;
    int final;
    int tamano;
    int capacidade;
    int capacidad;

public:
    Cola(int cap = 5){
        if (cap <= 0) cap = 5;
        capacidad = cap;
        frente = 0;
        final = 0;
        tamano = 0;

        datos.fijarTamano(capacidad);
    }

    bool encolar(T elemento){
        if (estaLlena()){
            return false;
        }
        datos.asignarEn(final, elemento);
        final = (final + 1) % capacidad;
        tamano++;
        return true;
    }

    void desencolar(){
        if(estaVacia()){
            std::cerr << "Error: Cola vacia, no es posible desencolar. \n";
            return;
        }
        frente = (frente + 1) % capacidad;
        tamano--;
    }

    T frenteActual() const{
        if (estaVacia()){
            std::cerr << "Error: Cola vacia, no hay frente. \n";
            std::exit(1);
        }
        return datos.obtenerEn(frente);
    }
    
    bool estaVacia() const{
        return tamano == 0;
    }

    bool estaLlena() const{
        return tamano == capacidad;
    }

    int tamanoActual() const{
        return tamano;
    }

    int capacidadActual() const{
        return capacidad;
    }
};

#endif
