#include <iostream>
#include <fstream>
#include <string>


std::string extraerCampo(const std::string& linea, const std::string& clave) {
    std::string patron = "\"" + clave + "\":";
    size_t pos = linea.find(patron);
    if (pos == std::string::npos) return "?";

    pos += patron.size();
    while (pos < linea.size() && linea[pos] == ' ') pos++;

    if (linea[pos] == '"') {
        pos++;
        size_t fin = linea.find('"', pos);
        return linea.substr(pos, fin - pos);
    }

    size_t fin = linea.find_first_of(",}", pos);
    return linea.substr(pos, fin - pos);
}

int main(int argc, char* argv[]) {
    std::string nombreArchivo = (argc > 1) ? argv[1] : "blockchain1.ndjson";

    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo abrir el archivo '" << nombreArchivo << "'\n";
        return 1;
    }

    std::string linea;
    int numBloque = 0;
    int numTx = 0;
    bool dentroDeBloque = false;
    std::string hashDelBloqueAnterior = "";

    std::cout << "=====================================\n";
    std::cout << "  Contenido del registro blockchain  \n";
    std::cout << "=====================================\n";

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue; 

        if (linea[0] == '{') {
            numBloque++;
            numTx = 0;

            std::string height    = extraerCampo(linea, "height");
            std::string prevHash  = extraerCampo(linea, "prev_hash");
            std::string blockHash = extraerCampo(linea, "block_hash");
            std::string txCount   = extraerCampo(linea, "tx_count");
            std::string ejemplo   = extraerCampo(linea, "ejemplo"); 
            std::cout << "\n-------------------------------------\n";
            std::cout << "BLOQUE #" << height << "\n";
            std::cout << "  prev_hash  : " << prevHash  << "\n";
            std::cout << "  block_hash : " << blockHash << "\n";
            std::cout << "  tx_count   : " << txCount   << "\n";
            if (ejemplo != "?") {
                std::cout << "  ejemplo    : " << ejemplo << "\n";
            }

            if (numBloque == 1) {
                std::cout << "  encadenamiento: (primer bloque, nada que comparar)\n";
            } else if (prevHash == hashDelBloqueAnterior) {
                std::cout << "  encadenamiento: OK\n";
            } else {
                std::cout << "  encadenamiento: NO COINCIDE\n";
            }
            hashDelBloqueAnterior = blockHash;

            std::cout << "  Transacciones:\n";
            dentroDeBloque = true;

        } else if (linea[0] == ' ' && dentroDeBloque) {
            numTx++;

            std::string txId        = extraerCampo(linea, "tx_id");
            std::string operacion   = extraerCampo(linea, "operacion");
            std::string idUsuario    = extraerCampo(linea, "id_usuario");
            std::string monto= extraerCampo(linea, "monto");
            std::string marcaTiempo = extraerCampo(linea, "marca_tiempo"); 

            std::cout << "    TX " << numTx << ": [" << txId << "]"
                      << " | " << operacion
                      << " | Usuario=" << idUsuario
                      << " | $" << monto
                      << " | t=" << marcaTiempo;
           
            std::cout << "\n";
        }
    }

    std::cout << "-------------------------------------\n\n";
    std::cout << "Total bloques leidos: " << numBloque << "\n";

    archivo.close();
    return 0;
}