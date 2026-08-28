#ifndef BT_SOLVER_HPP
#define BT_SOLVER_HPP

#include <string>
#include <vector>

struct Politica {
    int n = 8;
    int minLower = 2;
    int minUpper = 1;
    int minDigit = 1;
    int minSymbol = 1;
};

struct EstadoParcial {
    std::string prefijo = "";
    int count_lower = 0;
    int count_upper = 0;
    int count_digit = 0;
    int count_symbol = 0;
    char ultimo_caracter = '\0';
};

struct MetricasBT {
    long long nodos_visitados = 0;
    long long soluciones_encontradas = 0;
    double tiempo_ms = 0.0;
};

bool esFactible(const EstadoParcial& estado, const Politica& pol);

void backtrackingPoda(EstadoParcial estado, 
                     const Politica& pol, 
                     const std::string& alfabeto, 
                     MetricasBT& metricas);

#endif