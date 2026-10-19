#ifndef BLOQUE_H
#define BLOQUE_H
 
#include "EncabezadoBloque.h"
#include "Transaccion.h"
#include "DArray.h"
 

struct Bloque {
    EncabezadoBloque encabezado;
    DArray<Transaccion*> txList;
 
    Bloque() {
        encabezado.height = 0;
        encabezado.txCount = 0;
    }
};
 
#endif