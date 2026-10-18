#ifndef DARRAY_H
#define DARRAY_H

#include <iostream>
#include <cstdlib>

template <typename T>
class DArray {
    T* datos;
    int capacidad;
    int numElementos;

    void redimensionar(int nuevaCapacidad) {
        T* nuevo = new T[nuevaCapacidad]();
        for (int i = 0; i < numElementos; i++) {
            nuevo[i] = datos[i];
        }
        delete[] datos;
        datos = nuevo;
        capacidad = nuevaCapacidad;
    }

    void verificarIndice(int i) const {
        if (i < 0 || i >= numElementos) {
            std::cerr << "DArray: indice fuera de rango (" << i
                      << ", tamano=" << numElementos << ")" << std::endl;
            std::exit(1);
        }
    }

public:
    DArray() {
        capacidad = 4;
        numElementos = 0;
        datos = new T[capacidad]();
    }

    DArray(const DArray& otro) {
        capacidad = otro.capacidad;
        numElementos = otro.numElementos;
        datos = new T[capacidad]();
        for (int i = 0; i < numElementos; i++) {
            datos[i] = otro.datos[i];
        }
    }

    DArray& operator=(const DArray& otro) {
        if (this != &otro) {
            delete[] datos;
            capacidad = otro.capacidad;
            numElementos = otro.numElementos;
            datos = new T[capacidad]();
            for (int i = 0; i < numElementos; i++) {
                datos[i] = otro.datos[i];
            }
        }
        return *this;
    }

    ~DArray() {
        delete[] datos;
    }

    void agregar(T elemento) {
        if (numElementos == capacidad) {
            redimensionar(capacidad * 2);
        }
        datos[numElementos] = elemento;
        numElementos++;
    }

    void eliminarUltimo() {
        if (numElementos > 0) numElementos--;
    }

    void fijarTamano(int n) {
        if (n < 0) n = 0;
        if (n > capacidad) redimensionar(n);
        for (int i = numElementos; i < n; i++) {
            datos[i] = T();
        }
        numElementos = n;
    }

    T obtenerEn(int i) const {
        verificarIndice(i);
        return datos[i];
    }

    void asignarEn(int i, T valor) {
        verificarIndice(i);
        datos[i] = valor;
    }

    int size() const {
        return numElementos;
    }

    T& operator[](int i) {
        return datos[i];
    }
};

#endif
