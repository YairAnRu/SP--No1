#ifndef DARRAY_H
#define DARRAY_H

template <typename T>
class DArray {
    T* datos;
    int capacidad;
    int numElementos;

    void redimensionar() {
        capacidad *= 2;
        T* nuevo = new T[capacidad];
        for (int i = 0; i < numElementos; i++) {
            nuevo[i] = datos[i];
        }
        delete[] datos;
        datos = nuevo;
    }

public:
    DArray() {
        capacidad = 4;
        numElementos = 0;
        datos = new T[capacidad];
    }

    ~DArray() {
        delete[] datos;
    }

    void agregar(T elemento) {
        if (numElementos == capacidad) {
            redimensionar();
        }
        datos[numElementos] = elemento;
        numElementos++;
    }

    int size() {
        return numElementos;
    }

    T& operator[](int i) {
        return datos[i];
    }
};

#endif