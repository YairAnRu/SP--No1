#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "Transaccion.h"
#include "EncabezadoBloque.h"
#include "DArray.h"
#include "Bloque.h"
#include "ListaEnlazada.h"
#include "Pila.h"
#include "Cola.h"
using std::cout;
using std::endl;

bool extraerCampo(const char* linea, const char* clave, char* salida, int maxLen) {
    char patron[64];
    if (std::strlen(clave) + 4 > sizeof(patron)) {
        salida[0] = '\0';
        return false;
    }
    patron[0] = '"';
    std::strcpy(patron + 1, clave);
    std::strcat(patron, "\":");

    const char* pos = std::strstr(linea, patron);
    if (pos == nullptr) {
        salida[0] = '\0';
        return false;
    }
    pos += std::strlen(patron);
    while (*pos == ' ') pos++;

    int i = 0;
    if (*pos == '"') {
        pos++;
        while (*pos != '\0' && *pos != '"' && i < maxLen - 1) salida[i++] = *pos++;
    } else {
        while (*pos != '\0' && *pos != ',' && *pos != '}' && i < maxLen - 1) salida[i++] = *pos++;
    }
    salida[i] = '\0';
    return true;
}

bool cargarNDJSON(const char* nombreArchivo, ListaEnlazada<Bloque>& cadena) {
    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo abrir '" << nombreArchivo << "'" << endl;
        return false;
    }

    char linea[512];
    char campo[128];
    Bloque actual;
    bool hayBloque = false;

    while (archivo.getline(linea, sizeof(linea))) {
        int len = (int)std::strlen(linea);
        if (len > 0 && linea[len - 1] == '\r') linea[len - 1] = '\0';
        if (linea[0] == '\0') continue;

        if (linea[0] == '{') {
            if (hayBloque) {
                if (actual.txList.size() != actual.encabezado.txCount) {
                    std::cerr << "Aviso: bloque #" << actual.encabezado.height << " declara tx_count="
                              << actual.encabezado.txCount << " pero tiene " << actual.txList.size()
                              << " transacciones" << endl;
                }
                cadena.agregarInicio(actual);
            }
            actual = Bloque();
            hayBloque = true;
            extraerCampo(linea, "height", campo, sizeof(campo));
            actual.encabezado.height = std::atoi(campo);
            extraerCampo(linea, "tx_count", campo, sizeof(campo));
            actual.encabezado.txCount = std::atoi(campo);

        } else if ((linea[0] == ' ' || linea[0] == '\t') && hayBloque) {
            char txId[64], operacion[64], idUsuario[64], monto[64], marca[64];
            extraerCampo(linea, "tx_id", txId, sizeof(txId));
            extraerCampo(linea, "operacion", operacion, sizeof(operacion));
            extraerCampo(linea, "id_usuario", idUsuario, sizeof(idUsuario));
            extraerCampo(linea, "monto", monto, sizeof(monto));
            extraerCampo(linea, "marca_tiempo", marca, sizeof(marca));
            actual.txList.agregar(new Transaccion(txId, operacion, idUsuario,
                                                  std::atof(monto), std::atoll(marca)));
        }
    }
    if (hayBloque) {
        if (actual.txList.size() != actual.encabezado.txCount) {
            std::cerr << "Aviso: bloque #" << actual.encabezado.height << " declara tx_count="
                      << actual.encabezado.txCount << " pero tiene " << actual.txList.size()
                      << " transacciones" << endl;
        }
        cadena.agregarInicio(actual);
    }
    archivo.close();
    return true;
}

void liberarTransacciones(ListaEnlazada<Bloque>& cadena) {
    cadena.paraCada([](Bloque& b) {
        for (int i = 0; i < b.txList.size(); i++) {
            delete b.txList[i];
            b.txList[i] = nullptr;
        }
    });
}

void imprimirBloque(Bloque& b) {
    cout << "BLOQUE #" << b.encabezado.height << endl;
    cout << "  tx_count: " << b.encabezado.txCount << endl;
    cout << "  Transacciones:" << endl;
    for (int i = 0; i < b.txList.size(); i++) {
        cout << "    TX " << (i + 1) << ": ";
        b.txList[i]->imprimir();
    }
}

void titulo(const char* texto) {
    cout << "\n=====================================" << endl;
    cout << " " << texto << endl;
    cout << "=====================================" << endl;
}

void demoMempool(int alturaTip) {
    titulo("2) Mempool (Cola circular, capacidad 5)");

    const int CAPACIDAD = 5;
    const int TOTAL = 9;
    Cola<Transaccion*> mempool(CAPACIDAD);

    Transaccion* creadas[TOTAL];
    creadas[0] = new Transaccion("tx_101", "DEPOSITO", "U005", 1500.0, 1759600000);
    creadas[1] = new Transaccion("tx_102", "PAGO",     "U001",  320.5, 1759600010);
    creadas[2] = new Transaccion("tx_103", "RETIRO",   "U003",  800.0, 1759600020);
    creadas[3] = new Transaccion("tx_104", "PAGO",     "U004",   75.0, 1759600030);
    creadas[4] = new Transaccion("tx_105", "DEPOSITO", "U002", 2200.0, 1759600040);
    creadas[5] = new Transaccion("tx_106", "PAGO",     "U005",  199.9, 1759600050);
    creadas[6] = new Transaccion("tx_107", "RETIRO",   "U001",  500.0, 1759600060);
    creadas[7] = new Transaccion("tx_108", "DEPOSITO", "U006",  950.0, 1759600070);
    creadas[8] = new Transaccion("tx_109", "PAGO",     "U003",  410.0, 1759600080);

    cout << "\n[a] Encolando 5 transacciones:" << endl;
    for (int i = 0; i < 5; i++) {
        bool ok = mempool.encolar(creadas[i]);
        cout << "  encolar " << creadas[i]->txId << " -> " << (ok ? "OK" : "RECHAZADA")
             << "  (tam " << mempool.tamanoActual() << "/" << mempool.capacidadActual() << ")" << endl;
    }

    cout << "\n[b] Mempool llena? " << (mempool.estaLlena() ? "SI" : "NO")
         << ". Intentando encolar una transaccion mas:" << endl;
    bool ok = mempool.encolar(creadas[5]);
    cout << "  encolar " << creadas[5]->txId << " -> " << (ok ? "OK" : "RECHAZADA (mempool llena)") << endl;

    cout << "\n[c] El lider extrae 3 transacciones en orden FIFO para el bloque candidato:" << endl;
    Bloque candidato;
    candidato.encabezado.height = alturaTip + 1;
    for (int i = 0; i < 3 && !mempool.estaVacia(); i++) {
        Transaccion* t = mempool.frenteActual();
        mempool.desencolar();
        candidato.txList.agregar(t);
    }
    candidato.encabezado.txCount = candidato.txList.size();
    cout << "  (el candidato no se agrega a la cadena: la validacion real llega en la Entrega 3)" << endl;
    imprimirBloque(candidato);
    cout << "  Quedan en mempool: " << mempool.tamanoActual() << endl;

    cout << "\n[d] Segunda vuelta: la cola reutiliza las posiciones liberadas (aritmetica modular)" << endl;
    for (int i = 6; i < TOTAL; i++) {
        bool encolada = mempool.encolar(creadas[i]);
        cout << "  encolar " << creadas[i]->txId << " -> " << (encolada ? "OK" : "RECHAZADA")
             << "  (tam " << mempool.tamanoActual() << "/" << mempool.capacidadActual() << ")" << endl;
    }
    cout << "  Vaciando la mempool (debe salir en el mismo orden en que entro):" << endl;
    while (!mempool.estaVacia()) {
        cout << "    ";
        mempool.frenteActual()->imprimir();
        mempool.desencolar();
    }
    cout << "  Mempool vacia? " << (mempool.estaVacia() ? "SI" : "NO") << endl;

    for (int i = 0; i < TOTAL; i++) delete creadas[i];
}

bool bloqueValido(const Bloque& b, int alturaFalla) {
    return b.encabezado.height != alturaFalla;
}

void validarHaciaAtras(ListaEnlazada<Bloque>& cadena, int alturaFalla) {
    Pila<int> ruta;
    bool fallo = false;
    int alturaFallida = -1;

    cadena.paraCadaMientras([&](Bloque& b) -> bool {
        ruta.apilar(b.encabezado.height);
        if (!bloqueValido(b, alturaFalla)) {
            fallo = true;
            alturaFallida = b.encabezado.height;
            return false;
        }
        return true;
    });

    if (fallo) {
        cout << "  FALLA detectada en el bloque #" << alturaFallida << endl;
        cout << "  Ruta recorrida (desapilando, del bloque que fallo hacia el tip):" << endl;
    } else {
        cout << "  Cadena valida: se llego hasta el genesis" << endl;
        cout << "  Ruta recorrida (desapilando, del genesis hacia el tip):" << endl;
    }
    cout << "   ";
    while (!ruta.estaVacia()) {
        cout << " #" << ruta.tope();
        ruta.desapilar();
        if (!ruta.estaVacia()) cout << " ->";
    }
    cout << endl;
}

void demoRollback(ListaEnlazada<Bloque>& cadena) {
    titulo("3) Rollback path (Pila<int>)");
    cout << "\n[a] Sin fallas simuladas:" << endl;
    validarHaciaAtras(cadena, -1);
    cout << "\n[b] Falla simulada en el bloque intermedio #1:" << endl;
    validarHaciaAtras(cadena, 1);
}

void demoTxListEnlazada(ListaEnlazada<Bloque>& cadena) {
    titulo("4) Extra: tx_list con DArray vs ListaEnlazada");

    Bloque& tip = cadena.obtenerEn(0);
    ListaEnlazada<Transaccion*> lista;
    for (int i = 0; i < tip.txList.size(); i++) {
        lista.agregarFinal(tip.txList[i]);
    }

    cout << "\nTransacciones del bloque #" << tip.encabezado.height
         << " recorridas con paraCada() sobre la ListaEnlazada:" << endl;
    lista.paraCada([](Transaccion*& t) { cout << "  "; t->imprimir(); });

    cout << "\nAcceso por indice (DArray O(1)  vs  ListaEnlazada O(n)), posicion 1:" << endl;
    cout << "  DArray        : "; tip.txList[1]->imprimir();
    cout << "  ListaEnlazada : "; lista.obtenerEn(1)->imprimir();

    cout << "\nRegla de los 3: la copia es independiente de la original" << endl;
    ListaEnlazada<Transaccion*> copia(lista);
    copia.eliminar(tip.txList[0]);
    cout << "  tamano original = " << lista.tamanoActual()
         << " | tamano copia (tras eliminar 1) = " << copia.tamanoActual() << endl;
}

int main(int argc, char* argv[]) {
    const char* archivo = (argc > 1) ? argv[1] : "blockchain.ndjson";

    titulo("1) Carga del NDJSON (tip -> genesis)");
    ListaEnlazada<Bloque> cadena;
    if (!cargarNDJSON(archivo, cadena)) return 1;
    cout << "Bloques cargados: " << cadena.tamanoActual() << "\n" << endl;
    cadena.paraCada(imprimirBloque);

    int alturaTip = cadena.estaVacia() ? -1 : cadena.obtenerEn(0).encabezado.height;

    demoMempool(alturaTip);
    demoRollback(cadena);
    if (!cadena.estaVacia()) demoTxListEnlazada(cadena);

    liberarTransacciones(cadena);
    cout << "\nFin del programa." << endl;
    return 0;
}