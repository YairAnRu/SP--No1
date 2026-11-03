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
#include "TablaHash.h"
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

bool cargarNDJSON(const char* nombreArchivo, ListaEnlazada<Bloque>& cadena,
                  TablaHash<bool>& txExiste, TablaHash<int>& txLedger) {
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
            txExiste.insertar(txId, true);
            txLedger.insertar(txId, actual.encabezado.height);
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

void escribirNumero(char* destino, long long valor, int digitos) {
    for (int i = digitos - 1; i >= 0; i--) {
        destino[i] = (char)('0' + valor % 10);
        valor /= 10;
    }
}

void generarTxId(char* destino, int capacidad, const char* nodo, int secuencia, long long marcaTiempo) {
    char marca[24];
    int nDigitos = 0;
    long long m = marcaTiempo;
    if (m <= 0) marca[nDigitos++] = '0';
    while (m > 0) {
        marca[nDigitos++] = (char)('0' + m % 10);
        m /= 10;
    }
    int largoNodo = (int)std::strlen(nodo);
    int total = largoNodo + 1 + 5 + 1 + nDigitos;
    if (total + 1 > capacidad) {
        destino[0] = '\0';
        return;
    }
    int pos = 0;
    for (int i = 0; i < largoNodo; i++) destino[pos++] = nodo[i];
    destino[pos++] = '-';
    escribirNumero(destino + pos, secuencia, 5);
    pos += 5;
    destino[pos++] = '-';
    for (int i = nDigitos - 1; i >= 0; i--) destino[pos++] = marca[i];
    destino[pos] = '\0';
}

Transaccion* crearTransaccion(int& secuencia, const char* operacion, const char* usuario,
                              double monto, long long marcaTiempo) {
    char id[64];
    secuencia++;
    generarTxId(id, sizeof(id), "NOD001", secuencia, marcaTiempo);
    return new Transaccion(id, operacion, usuario, monto, marcaTiempo);
}

void demoMempool(int alturaTip, int& secuencia) {
    titulo("2) Mempool (Cola circular, capacidad 5)");

    const int CAPACIDAD = 5;
    const int TOTAL = 9;
    Cola<Transaccion*> mempool(CAPACIDAD);

    Transaccion* creadas[TOTAL];
    creadas[0] = crearTransaccion(secuencia, "DEPOSITO", "U005", 1500.0, 1759600000);
    creadas[1] = crearTransaccion(secuencia, "PAGO",     "U001",  320.5, 1759600010);
    creadas[2] = crearTransaccion(secuencia, "RETIRO",   "U003",  800.0, 1759600020);
    creadas[3] = crearTransaccion(secuencia, "PAGO",     "U004",   75.0, 1759600030);
    creadas[4] = crearTransaccion(secuencia, "DEPOSITO", "U002", 2200.0, 1759600040);
    creadas[5] = crearTransaccion(secuencia, "PAGO",     "U005",  199.9, 1759600050);
    creadas[6] = crearTransaccion(secuencia, "RETIRO",   "U001",  500.0, 1759600060);
    creadas[7] = crearTransaccion(secuencia, "DEPOSITO", "U006",  950.0, 1759600070);
    creadas[8] = crearTransaccion(secuencia, "PAGO",     "U003",  410.0, 1759600080);

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

int consultarBloque(TablaHash<int>& txLedger, const char* txId) {
    int* altura = txLedger.buscar(txId);
    if (altura == nullptr) return -1;
    return *altura;
}

Bloque* buscarBloquePorAltura(ListaEnlazada<Bloque>& cadena, int altura) {
    Bloque* encontrado = nullptr;
    cadena.paraCadaMientras([&](Bloque& b) -> bool {
        if (b.encabezado.height == altura) {
            encontrado = &b;
            return false;
        }
        return true;
    });
    return encontrado;
}

void mostrarConsulta(ListaEnlazada<Bloque>& cadena, TablaHash<int>& txLedger, const char* txId) {
    cout << "Consulta por txId: " << txId << endl;
    int altura = consultarBloque(txLedger, txId);
    if (altura == -1) {
        cout << "  [NO ENCONTRADO] el txId " << txId << " no existe en el ledger (consultarBloque devolvio -1)" << endl;
        return;
    }
    cout << "  Altura devuelta por txLedger: " << altura << endl;
    Bloque* b = buscarBloquePorAltura(cadena, altura);
    if (b == nullptr) {
        cout << "  [ERROR] el indice apunta a la altura " << altura << " pero ese bloque no esta en la cadena" << endl;
        return;
    }
    for (int i = 0; i < b->txList.size(); i++) {
        if (std::strcmp(b->txList[i]->txId, txId) == 0) {
            cout << "  Transaccion encontrada: ";
            b->txList[i]->imprimir();
        }
    }
    cout << "  Bloque donde esta almacenada:" << endl;
    imprimirBloque(*b);
}

bool encolarSinDuplicado(Cola<Transaccion*>& mempool, Transaccion* t, TablaHash<bool>& txExiste) {
    bool* existe = txExiste.buscar(t->txId);
    if (existe != nullptr && *existe) {
        cout << "  [ERROR] txId duplicado: " << t->txId << " — transacción rechazada" << endl;
        return false;
    }
    if (!mempool.encolar(t)) {
        cout << "  [ERROR] mempool llena: " << t->txId << " no se pudo encolar" << endl;
        return false;
    }
    cout << "  txId " << t->txId << " no existe en el indice -> encolada (mempool " << mempool.tamanoActual()
         << "/" << mempool.capacidadActual() << ")" << endl;
    return true;
}

void demoDuplicados(TablaHash<bool>& txExiste, int& secuencia) {
    titulo("5) Indice 1: deteccion de duplicados (txExiste)");

    Cola<Transaccion*> mempool(5);
    Transaccion* intentos[3];
    char idReenvio[64];
    char idInventada[64];
    generarTxId(idReenvio, sizeof(idReenvio), "NOD001", 3, 1759590600);
    generarTxId(idInventada, sizeof(idInventada), "NOD001", 8, 1759593000);
    intentos[0] = new Transaccion(idReenvio, "PAGO", "U001", 450.75, 1759590600);
    intentos[1] = new Transaccion(idInventada, "RETIRO", "U999", 99999.0, 1759700000);
    intentos[2] = crearTransaccion(secuencia, "DEPOSITO", "U007", 640.0, 1759700100);

    cout << "\n[a] Reenvio de una transaccion que ya esta en el NDJSON (" << intentos[0]->txId << "):" << endl;
    cout << "  Intento: ";
    intentos[0]->imprimir();
    encolarSinDuplicado(mempool, intentos[0], txExiste);

    cout << "\n[b] Transaccion inventada que reutiliza un txId existente (" << intentos[1]->txId << "):" << endl;
    cout << "  Intento: ";
    intentos[1]->imprimir();
    encolarSinDuplicado(mempool, intentos[1], txExiste);

    cout << "\n[c] Transaccion nueva con txId que no existe (" << intentos[2]->txId << "):" << endl;
    cout << "  Intento: ";
    intentos[2]->imprimir();
    encolarSinDuplicado(mempool, intentos[2], txExiste);

    cout << "\n  Transacciones en la mempool al final: " << mempool.tamanoActual() << endl;

    while (!mempool.estaVacia()) mempool.desencolar();
    for (int i = 0; i < 3; i++) delete intentos[i];
}

void demoConsultas(ListaEnlazada<Bloque>& cadena, TablaHash<int>& txLedger) {
    titulo("6) Indice 2: consultarBloque(txId) (txLedger)");

    cout << "\n[a] txId que existe:" << endl;
    mostrarConsulta(cadena, txLedger, "NOD001-00005-1759590720");

    cout << "\n[b] Otro txId que existe (en el tip):" << endl;
    mostrarConsulta(cadena, txLedger, "NOD001-00009-1759593060");

    cout << "\n[c] txId que no existe:" << endl;
    mostrarConsulta(cadena, txLedger, "NOD001-99999-1759999999");
}

void demoFlujoCompleto(ListaEnlazada<Bloque>& cadena, TablaHash<bool>& txExiste, TablaHash<int>& txLedger, int& secuencia) {
    titulo("7) Flujo completo: Mempool -> bloque -> indices");

    Cola<Transaccion*> mempool(5);

    cout << "\n[1] Se crea una transaccion nueva con txId unico:" << endl;
    Transaccion* nueva = crearTransaccion(secuencia, "PAGO", "U002", 275.5, 1759800000);
    cout << "  ";
    nueva->imprimir();

    cout << "\n[2] Se verifica en txExiste y se encola en la Mempool:" << endl;
    if (!encolarSinDuplicado(mempool, nueva, txExiste)) {
        delete nueva;
        return;
    }

    cout << "\n[3] Confirmacion: el lider mueve las transacciones de la Mempool a un bloque nuevo:" << endl;
    int alturaTip = cadena.estaVacia() ? -1 : cadena.obtenerEn(0).encabezado.height;
    Bloque nuevoBloque;
    nuevoBloque.encabezado.height = alturaTip + 1;
    while (!mempool.estaVacia()) {
        Transaccion* t = mempool.frenteActual();
        mempool.desencolar();
        nuevoBloque.txList.agregar(t);
    }
    nuevoBloque.encabezado.txCount = nuevoBloque.txList.size();
    cadena.agregarInicio(nuevoBloque);
    cout << "  Bloque #" << nuevoBloque.encabezado.height << " agregado a la cadena con "
         << nuevoBloque.encabezado.txCount << " transaccion(es). Mempool: " << mempool.tamanoActual() << endl;

    cout << "\n[4] Se insertan los txId del bloque en ambas tablas:" << endl;
    for (int i = 0; i < nuevoBloque.txList.size(); i++) {
        const char* id = nuevoBloque.txList[i]->txId;
        txExiste.insertar(id, true);
        txLedger.insertar(id, nuevoBloque.encabezado.height);
        cout << "  txExiste[" << id << "] = true | txLedger[" << id << "] = " << nuevoBloque.encabezado.height << endl;
    }

    cout << "\n[5] Verificacion con consultarBloque:" << endl;
    mostrarConsulta(cadena, txLedger, nueva->txId);

    cout << "\n[6] Si la misma transaccion se vuelve a enviar, ahora es duplicado:" << endl;
    Transaccion* repetida = new Transaccion(nueva->txId, "PAGO", "U002", 275.5, 1759800000);
    encolarSinDuplicado(mempool, repetida, txExiste);
    delete repetida;
}

int main(int argc, char* argv[]) {
    const char* archivo = (argc > 1) ? argv[1] : "blockchain.ndjson";

    titulo("1) Carga del NDJSON y construccion de indices (tip -> genesis)");
    ListaEnlazada<Bloque> cadena;
    TablaHash<bool> txExiste;
    TablaHash<int> txLedger;
    if (!cargarNDJSON(archivo, cadena, txExiste, txLedger)) return 1;
    cout << "Bloques cargados: " << cadena.tamanoActual() << "\n" << endl;
    cadena.paraCada(imprimirBloque);

    int totalTx = 0;
    cadena.paraCada([&](Bloque& b) { totalTx += b.txList.size(); });
    cout << "\nIndexadas: " << txExiste.tamano() << " de " << totalTx << " transacciones en "
         << cadena.tamanoActual() << " bloques (txExiste: " << txExiste.tamano()
         << " claves | txLedger: " << txLedger.tamano() << " claves)" << endl;
    if (txExiste.tamano() != totalTx) {
        cout << "[AVISO] hay txId repetidos en el archivo NDJSON" << endl;
    }

    int alturaTip = cadena.estaVacia() ? -1 : cadena.obtenerEn(0).encabezado.height;
    int secuencia = totalTx;

    demoMempool(alturaTip, secuencia);
    demoRollback(cadena);
    if (!cadena.estaVacia()) demoTxListEnlazada(cadena);

    demoDuplicados(txExiste, secuencia);
    demoConsultas(cadena, txLedger);
    demoFlujoCompleto(cadena, txExiste, txLedger, secuencia);

    titulo("8) Resumen de los indices");
    cout << "Bloques en la cadena: " << cadena.tamanoActual() << endl;
    cout << "txExiste : " << txExiste.tamano() << " claves | capacidad " << txExiste.capacidad()
         << " | factor de carga " << txExiste.factorCarga() << endl;
    cout << "txLedger : " << txLedger.tamano() << " claves | capacidad " << txLedger.capacidad()
         << " | factor de carga " << txLedger.factorCarga() << endl;

    liberarTransacciones(cadena);
    cout << "\nFin del programa." << endl;
    return 0;
}