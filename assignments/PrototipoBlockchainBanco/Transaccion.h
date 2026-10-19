#ifndef TRANSACCION_H
#define TRANSACCION_H

#include <iostream>
#include <cstring>

struct Transaccion {
    char* txId;
    char* operacion;
    char* idUsuario;
    double monto;
    long long marcaTiempo;

    Transaccion() {
        txId = new char[1];
        operacion = new char[1];
        idUsuario = new char[1];
        txId[0] = '\0';
        operacion[0] = '\0';
        idUsuario[0] = '\0';
        monto = 0.0;
        marcaTiempo = 0;
    }

    Transaccion(const char* id, const char* op, const char* usr, double m, long long t) {
        txId = new char[strlen(id) + 1];
        strcpy(txId, id);
        operacion = new char[strlen(op) + 1];
        strcpy(operacion, op);
        idUsuario = new char[strlen(usr) + 1];
        strcpy(idUsuario, usr);
        monto = m;
        marcaTiempo = t;
    }

    Transaccion(const Transaccion& otra) {
        txId = new char[strlen(otra.txId) + 1];
        strcpy(txId, otra.txId);
        operacion = new char[strlen(otra.operacion) + 1];
        strcpy(operacion, otra.operacion);
        idUsuario = new char[strlen(otra.idUsuario) + 1];
        strcpy(idUsuario, otra.idUsuario);
        monto = otra.monto;
        marcaTiempo = otra.marcaTiempo;
    }

    Transaccion& operator=(const Transaccion& otra) {
        if (this != &otra) {
            delete[] txId;
            delete[] operacion;
            delete[] idUsuario;
            txId = new char[strlen(otra.txId) + 1];
            strcpy(txId, otra.txId);
            operacion = new char[strlen(otra.operacion) + 1];
            strcpy(operacion, otra.operacion);
            idUsuario = new char[strlen(otra.idUsuario) + 1];
            strcpy(idUsuario, otra.idUsuario);
            monto = otra.monto;
            marcaTiempo = otra.marcaTiempo;
        }
        return *this;
    }

    ~Transaccion() {
        delete[] txId;
        delete[] operacion;
        delete[] idUsuario;
    }

    void imprimir() {
        std::cout << "txId: " << txId
                  << " | operacion: " << operacion
                  << " | usuario: " << idUsuario
                  << " | monto: " << monto
                  << " | tiempo: " << marcaTiempo << std::endl;
    }
};

#endif