#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include "bt_solver.hpp"

int main() {
    
    // Alfabeto base reducido para prueba rápida
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

    // 1. Ejecución CON PODA
    auto inicio_poda = std::chrono::high_resolution_clock::now();
    backtrackingPoda(estado_inicial, pol, alfabeto, metricas_poda);
    auto fin_poda = std::chrono::high_resolution_clock::now();
    metricas_poda.tiempo_ms = std::chrono::duration<double, std::milli>(fin_poda - inicio_poda).count();

    // 2. Ejecución SIN PODA
    auto inicio_sin = std::chrono::high_resolution_clock::now();
    backtrackingSinPoda(estado_inicial, pol, alfabeto, metricas_sin_poda);
    auto fin_sin = std::chrono::high_resolution_clock::now();
    metricas_sin_poda.tiempo_ms = std::chrono::duration<double, std::milli>(fin_sin - inicio_sin).count();

   
    double reduccion = (1.0 - (double)metricas_poda.nodos_visitados / metricas_sin_poda.nodos_visitados) * 100.0;

    // Muestra de resultados
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