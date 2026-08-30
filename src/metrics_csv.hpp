// src/metrics_csv.hpp

#pragma once

#include <string>
#include <vector>

// Guarda los datos de cada ejecucion del Modulo FB.
struct RegistroFB {
    std::string instancia;          // nombre de la instancia
    std::string estrategia;         // fuerza_bruta o diccionario
    std::string alfabeto;           // A1, A2 o - si no aplica
    int n = 0;                      // longitud de la contrasena
    double tiempo_ms = 0.0;         // tiempo que tomo la ejecucion
    long long candidatos_evaluados = 0;
    bool encontrada = false;
    bool en_diccionario = false;    // indica si estaba en el diccionario
};

// Guarda los datos de cada ejecucion del Modulo BT.
struct RegistroBT {
    std::string instancia;              // nombre de la instancia
    std::string version;                // con_poda o sin_poda
    int n = 0;
    double tiempo_ms = 0.0;             // tiempo que tomo la ejecucion
    long long nodos_generados = 0;      // nodos generados o visitados
    long long soluciones_encontradas = 0;
};

// Guarda los resultados del Modulo FB en un archivo CSV.
void exportarCSV_FB(const std::vector<RegistroFB>& registros, const std::string& ruta);

// Guarda los resultados del Modulo BT en un archivo CSV.
// Tambien calcula la reduccion cuando se tienen las dos versiones.
void exportarCSV_BT(const std::vector<RegistroBT>& registros, const std::string& ruta);