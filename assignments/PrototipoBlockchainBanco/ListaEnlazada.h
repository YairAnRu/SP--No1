#ifndef LISTAENLAZADA_H
#define LISTAENLAZADA_H

#include <iostream>
#include <cstdlib>

template <typename T>
class ListaEnlazada {
    struct Nodo {
        T dato;
        Nodo* sig;
        Nodo(const T& d) : dato(d), sig(nullptr) {}
    };

    Nodo* cabeza;
    Nodo* cola;
    int tam;

    void vaciar() {
        Nodo* actual = cabeza;
        while (actual != nullptr) {
            Nodo* siguiente = actual->sig;
            delete actual;
            actual = siguiente;
        }
        cabeza = nullptr;
        cola = nullptr;
        tam = 0;
    }

public:
    ListaEnlazada() : cabeza(nullptr), cola(nullptr), tam(0) {}

    ListaEnlazada(const ListaEnlazada& otra) : cabeza(nullptr), cola(nullptr), tam(0) {
        for (Nodo* p = otra.cabeza; p != nullptr; p = p->sig) {
            agregarFinal(p->dato);          
        }
    }

    ListaEnlazada& operator=(const ListaEnlazada& otra) {
        if (this != &otra) {
            vaciar();
            for (Nodo* p = otra.cabeza; p != nullptr; p = p->sig) {
                agregarFinal(p->dato);
            }
        }
        return *this;
    }

    ~ListaEnlazada() {
        vaciar();
    }

    void agregarFinal(const T& valor) {
        Nodo* nuevo = new Nodo(valor);
        if (cola == nullptr) {
            cabeza = nuevo;
            cola = nuevo;
        } else {
            cola->sig = nuevo;
            cola = nuevo;
        }
        tam++;
    }

    void agregarInicio(const T& valor) {
        Nodo* nuevo = new Nodo(valor);
        nuevo->sig = cabeza;
        cabeza = nuevo;
        if (cola == nullptr) {
            cola = nuevo;
        }
        tam++;
    }

    bool eliminar(const T& valor) {
        Nodo* anterior = nullptr;
        Nodo* actual = cabeza;
        while (actual != nullptr) {
            if (actual->dato == valor) {
                if (anterior == nullptr) {
                    cabeza = actual->sig;
                } else {
                    anterior->sig = actual->sig;
                }
                if (actual == cola) {
                    cola = anterior;        
                }
                delete actual;
                tam--;
                return true;
            }
            anterior = actual;
            actual = actual->sig;
        }
        return false;
    }

    T& obtenerEn(int indice) {
        if (indice < 0 || indice >= tam) {
            std::cerr << "ListaEnlazada: indice fuera de rango (" << indice
                      << ", tamano=" << tam << ")" << std::endl;
            std::exit(1);
        }
        Nodo* p = cabeza;
        for (int i = 0; i < indice; i++) {
            p = p->sig;
        }
        return p->dato;
    }

    template <typename Funcion>
    void paraCada(Funcion f) {
        for (Nodo* p = cabeza; p != nullptr; p = p->sig) {
            f(p->dato);
        }
    }

    template <typename Funcion>
    void paraCadaMientras(Funcion f) {
        for (Nodo* p = cabeza; p != nullptr; p = p->sig) {
            if (!f(p->dato)) return;
        }
    }

    bool estaVacia() const { return tam == 0; }
    int tamanoActual() const { return tam; }
};

#endif
