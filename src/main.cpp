#include "bt_solver.hpp"
#include "fb_solver.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <cstdlib>

static void print_usage() {
    std::cout << "=== HERRAMIENTA ADA P1 ===" << std::endl;
    std::cout << "Uso para Backtracking:" << std::endl;
    std::cout << "  ./app bt" << std::endl;
    std::cout << "Uso para Fuerza Bruta / Diccionario:" << std::endl;
    std::cout << "  ./app <target_hash> brute <alphabet> <max_length>" << std::endl;
    std::cout << "  ./app <target_hash> dict <dictionary.txt>" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string modo = argv[1];

     //BACKTRACKING
     
    if (modo == "bt") {
        std::string alfabeto = "abcdefghijklmnopqrstuvwxyz0123456789!@#$";
        Politica pol;
        pol.n = 4;
        pol.minLower = 1;
        pol.minUpper = 0;
        pol.minDigit = 1;
        pol.minSymbol = 1;

        EstadoParcial estado_inicial;
        MetricasBT metricas_poda, metricas_sin_poda;

        std::cout << "=== COMPARATIVA MÓDULO BT (Sección 8.2) ===" << std::endl;

        auto inicio_poda = std::chrono::high_resolution_clock::now();
        backtrackingPoda(estado_inicial, pol, alfabeto, metricas_poda);
        auto fin_poda = std::chrono::high_resolution_clock::now();
        metricas_poda.tiempo_ms = std::chrono::duration<double, std::milli>(fin_poda - inicio_poda).count();

        auto inicio_sin = std::chrono::high_resolution_clock::now();
        backtrackingSinPoda(estado_inicial, pol, alfabeto, metricas_sin_poda);
        auto fin_sin = std::chrono::high_resolution_clock::now();
        metricas_sin_poda.tiempo_ms = std::chrono::duration<double, std::milli>(fin_sin - inicio_sin).count();

        double reduccion = (1.0 - (double)metricas_poda.nodos_visitados / metricas_sin_poda.nodos_visitados) * 100.0;

        std::cout << "\n------------------------------------------------" << std::endl;
        std::cout << "Métrica                        | Con Poda    | Sin Poda" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Soluciones encontradas         | " << metricas_poda.soluciones_encontradas << "       | " << metricas_sin_poda.soluciones_encontradas << std::endl;
        std::cout << "Nodos visitados/generados      | " << metricas_poda.nodos_visitados << "     | " << metricas_sin_poda.nodos_visitados << std::endl;
        std::cout << "Tiempo de ejecución (ms)       | " << std::fixed << std::setprecision(2) << metricas_poda.tiempo_ms << " ms   | " << metricas_sin_poda.tiempo_ms << " ms" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Reducción del espacio de búsqueda: " << std::setprecision(2) << reduccion << " %" << std::endl;

        return 0;
    }

    //FUERZA BRUTA
    if (argc < 3) {
        print_usage();
        return 1;
    }

    const std::string target_hash = argv[1];
    const std::string fb_mode = argv[2];

    if (fb_mode == "brute" || fb_mode == "alphabet") {
        const std::string alphabet = (argc > 3) ? argv[3] : "abcdefghijklmnopqrstuvwxyz";
        const std::size_t max_length = (argc > 4) ? static_cast<std::size_t>(std::stoul(argv[4])) : 4;

        const auto result = brute_force_search(target_hash, alphabet, max_length);
        if (result.found) {
            std::cout << "Encontrada: " << result.password << "\n";
            std::cout << "Pruebas: " << result.tested << "\n";
            return 0;
        }
        std::cout << "No se encontró ninguna contraseña en el rango indicado.\n";
        return 2;
    }

    if (fb_mode == "dict" || fb_mode == "dictionary") {
        const std::string dictionary_path = (argc > 3) ? argv[3] : "resources/wordlist.txt";
        const auto result = dictionary_attack(target_hash, dictionary_path);
        if (result.found) {
            std::cout << "Encontrada: " << result.password << "\n";
            return 0;
        }
        std::cout << "No se encontró la contraseña en el diccionario.\n";
        return 2;
    }

    std::cout << "Modo no soportado.\n";
    print_usage();
    return 1;
}