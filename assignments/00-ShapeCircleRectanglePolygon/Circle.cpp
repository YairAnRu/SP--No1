

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

const int MAX_LINEA = 512;

void extraerCampo(const char* linea, const char* campo, char* destino, int maxDest) {
    char patron[64];
    strcpy(patron, "\"");
    strcat(patron, campo);
    strcat(patron, "\":");

    const char* pos = strstr(linea, patron);
    if (pos == nullptr) {
        strcpy(destino, "?");
        return;
    }


    pos += strlen(patron);

    while (*pos == ' ') pos++;

    if (*pos == '"') {
        pos++;
        int i = 0;
        while (*pos != '"' && *pos != '\0' && i < maxDest - 1) {
            destino[i++] = *pos++;
        }
        destino[i] = '\0';
    } else {
        int i = 0;
        while (*pos != ',' && *pos != '}' && *pos != '\0' && i < maxDest - 1) {
            destino[i++] = *pos++;
        }
        destino[i] = '\0';
    }
}

int main(int argc, char* argv[]) {
    const char* nombreArchivo = (argc > 1) ? argv[1] : "blockchain1.ndjson";

    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo abrir el archivo '"
                  << nombreArchivo << "'" << std::endl;
        return 1;
    }

    char linea[MAX_LINEA];
    char campo[128];
    int numBloque = 0;
    int numTx     = 0;
    bool dentroDeBloque = false;

    std::cout << "=====================================" << std::endl;
    std::cout << "  Contenido del registro blockchain  " << std::endl;
    std::cout << "=====================================" << std::endl;

    while (archivo.getline(linea, MAX_LINEA)) {
        if (strlen(linea) == 0) continue;

       
        if (linea[0] == '{') {
        
            numBloque++;
            numTx = 0;

            extraerCampo(linea, "height",     campo, sizeof(campo));
            int height = atoi(campo);

            extraerCampo(linea, "prev_hash",  campo, sizeof(campo));
            char prevHash[128];
            strcpy(prevHash, campo);

            extraerCampo(linea, "block_hash", campo, sizeof(campo));
            char blockHash[128];
            strcpy(blockHash, campo);

            extraerCampo(linea, "tx_count",   campo, sizeof(campo));
            int txCount = atoi(campo);

            // Campo agregado al encabezado del bloque
            extraerCampo(linea, "ejemplo",   campo, sizeof(campo));
            int nuEjemplo = atoi(campo);

            std::cout << std::endl;
            std::cout << "-------------------------------------" << std::endl;
            std::cout << "BLOQUE #" << height << std::endl;
            std::cout << "  prev_hash  : " << prevHash  << std::endl;
            std::cout << "  block_hash : " << blockHash << std::endl;
            std::cout << "  tx_count   : " << txCount   << std::endl;
            std::cout << "  ejemplo   : " << nuEjemplo   << std::endl;
            std::cout << "  Transacciones:" << std::endl;

            dentroDeBloque = true;

        } else if (linea[0] == ' ' && dentroDeBloque) {
            numTx++;

            extraerCampo(linea, "tx_id",       campo, sizeof(campo));
            char txId[64];
            strcpy(txId, campo);

            extraerCampo(linea, "operacion",   campo, sizeof(campo));
            char operacion[32];
            strcpy(operacion, campo);

            extraerCampo(linea, "id_alumno",   campo, sizeof(campo));
            char idAlumno[32];
            strcpy(idAlumno, campo);

            extraerCampo(linea, "actividad",   campo, sizeof(campo));
            char actividad[32];
            strcpy(actividad, campo);

            extraerCampo(linea, "calificacion", campo, sizeof(campo));
            double calificacion = atof(campo);

            extraerCampo(linea, "marca_tiempo", campo, sizeof(campo));
            long marcaTiempo = atol(campo);

            extraerCampo(linea, "seccion",   campo, sizeof(campo));
            char seccion[2];
            strcpy(seccion, campo);

            std::cout << "    TX " << numTx << ": [" << txId << "]"
                      << " | " << operacion
                      << " | alumno=" << idAlumno
                      << " | act=" << actividad
                      << " | calif=" << calificacion
                      << " | t=" << marcaTiempo
                      << " | seccion=" << seccion
                      << std::endl;
        }
    }

    std::cout << "-------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "Total bloques leidos: " << numBloque << std::endl;

    archivo.close();
    return 0;
}