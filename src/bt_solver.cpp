#include "bt_solver.hpp"
#include <cctype>
#include <algorithm>

bool esFactible(const EstadoParcial& estado, const Politica& pol) {
    int faltantes_posiciones = pol.n - static_cast<int>(estado.prefijo.length());
    
    int faltan_lower  = std::max(0, pol.minLower - estado.count_lower);
    int faltan_upper  = std::max(0, pol.minUpper - estado.count_upper);
    int faltan_digit  = std::max(0, pol.minDigit - estado.count_digit);
    int faltan_symbol = std::max(0, pol.minSymbol - estado.count_symbol);
    
    int total_faltantes = faltan_lower + faltan_upper + faltan_digit + faltan_symbol;
    
    return total_faltantes <= faltantes_posiciones;
}

void backtrackingPoda(EstadoParcial estado, 
                     const Politica& pol, 
                     const std::string& alfabeto, 
                     MetricasBT& metricas) {
    
    metricas.nodos_visitados++;

    if (estado.prefijo.length() == static_cast<size_t>(pol.n)) {
        if (estado.count_lower >= pol.minLower &&
            estado.count_upper >= pol.minUpper &&
            estado.count_digit >= pol.minDigit &&
            estado.count_symbol >= pol.minSymbol) {
            
            metricas.soluciones_encontradas++; // Solo contamos en lugar de guardar cadenas en RAM
        }
        return;
    }

    for (char c : alfabeto) {
        if (c == estado.ultimo_caracter) continue;

        EstadoParcial nuevo_estado = estado;
        nuevo_estado.prefijo += c;
        nuevo_estado.ultimo_caracter = c;

        if (std::islower(static_cast<unsigned char>(c))) nuevo_estado.count_lower++;
        else if (std::isupper(static_cast<unsigned char>(c))) nuevo_estado.count_upper++;
        else if (std::isdigit(static_cast<unsigned char>(c))) nuevo_estado.count_digit++;
        else nuevo_estado.count_symbol++;

        if (esFactible(nuevo_estado, pol)) {
            backtrackingPoda(nuevo_estado, pol, alfabeto, metricas);
        }
    }
}