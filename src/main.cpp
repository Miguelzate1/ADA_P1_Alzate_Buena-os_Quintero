#include <iostream>
#include <string>
#include <chrono>
#include "bt_solver.hpp"

int main() {
    std::string alfabeto = "abcdefghijklmnopqrstuvwxyz"
                           "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                           "0123456789"
                           "!@#$%";

    Politica pol_ref;
    pol_ref.n = 6;
    pol_ref.minLower = 2;
    pol_ref.minUpper = 1;
    pol_ref.minDigit = 1;
    pol_ref.minSymbol = 1;

    EstadoParcial estado_inicial;
    MetricasBT metricas;

    std::cout << "=== PRUEBA MÓDULO BT (Instancia Referencia n=6) ===" << std::endl;

    auto inicio = std::chrono::high_resolution_clock::now();
    backtrackingPoda(estado_inicial, pol_ref, alfabeto, metricas);
    auto fin = std::chrono::high_resolution_clock::now();

    metricas.tiempo_ms = std::chrono::duration<double, std::milli>(fin - inicio).count();

    std::cout << "Soluciones encontradas: " << metricas.soluciones_encontradas << std::endl;
    std::cout << "Nodos visitados (con poda): " << metricas.nodos_visitados << std::endl;
    std::cout << "Tiempo de ejecución: " << metricas.tiempo_ms << " ms" << std::endl;

    return 0;
}