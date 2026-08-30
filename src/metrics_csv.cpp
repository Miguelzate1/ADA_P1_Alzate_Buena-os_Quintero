// src/metrics_csv.cpp

#include "metrics_csv.hpp"

#include <fstream>
#include <iostream>
#include <map>

void exportarCSV_FB(const std::vector<RegistroFB>& registros, const std::string& ruta) {
    // Abrimos el archivo donde vamos a guardar los resultados.
    std::ofstream archivo(ruta);

    if (!archivo) {
        std::cerr << "[metrics_csv] No se pudo abrir " << ruta << " para escritura\n";
        return;
    }

    // Escribimos los nombres de las columnas.
    archivo << "instancia,estrategia,alfabeto,n,tiempo_ms,candidatos_evaluados,"
               "encontrada,en_diccionario\n";

    // Guardamos cada registro como una fila del CSV.
    for (const auto& r : registros) {
        archivo << r.instancia << ","
                << r.estrategia << ","
                << r.alfabeto << ","
                << r.n << ","
                << r.tiempo_ms << ","
                << r.candidatos_evaluados << ","
                << (r.encontrada ? "si" : "no") << ","
                << (r.en_diccionario ? "si" : "no") << "\n";
    }

    std::cout << "[metrics_csv] Escritos " << registros.size()
               << " registros FB en " << ruta << "\n";
}

void exportarCSV_BT(const std::vector<RegistroBT>& registros, const std::string& ruta) {
    // Abrimos el archivo para guardar los resultados del BT.
    std::ofstream archivo(ruta);

    if (!archivo) {
        std::cerr << "[metrics_csv] No se pudo abrir " << ruta << " para escritura\n";
        return;
    }

    // Guardamos los nodos generados por la version sin poda.
    // Estos datos se usan despues para calcular la reduccion.
    std::map<std::string, long long> nodos_sin_poda;

    for (const auto& r : registros) {
        if (r.version == "sin_poda") {
            nodos_sin_poda[r.instancia] = r.nodos_generados;
        }
    }

    // Escribimos los nombres de las columnas.
    archivo << "instancia,version,n,tiempo_ms,nodos_generados,"
               "soluciones_encontradas,reduccion_pct\n";

    // Escribimos cada resultado en una fila.
    for (const auto& r : registros) {
        std::string reduccion_str = "";

        // La reduccion solo se calcula para la version con poda.
        if (r.version == "con_poda") {
            auto it = nodos_sin_poda.find(r.instancia);

            if (it != nodos_sin_poda.end() && it->second > 0) {
                double reduccion = 100.0 * (1.0 - (static_cast<double>(r.nodos_generados) /
                                                    static_cast<double>(it->second)));

                reduccion_str = std::to_string(reduccion);
            }
        }

        archivo << r.instancia << ","
                << r.version << ","
                << r.n << ","
                << r.tiempo_ms << ","
                << r.nodos_generados << ","
                << r.soluciones_encontradas << ","
                << reduccion_str << "\n";
    }

    std::cout << "[metrics_csv] Escritos " << registros.size()
               << " registros BT en " << ruta << "\n";
}