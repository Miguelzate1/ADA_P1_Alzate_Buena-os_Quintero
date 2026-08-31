

#include <iostream>
#include <cassert>
#include "bt_solver.hpp"
#include "fb_solver.hpp"

int main() {
    int fallos = 0;
--------
    {
        Politica pol_ref{6, 2, 1, 1, 1};
        std::string alfabeto =
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789!@#$%";

        EstadoParcial estado_inicial{};
        MetricasBT metricas_con_poda{};
        backtrackingPoda(estado_inicial, pol_ref, alfabeto, metricas_con_poda);

        EstadoParcial estado_inicial2{};
        MetricasBT metricas_sin_poda{};
        backtrackingSinPoda(estado_inicial2, pol_ref, alfabeto, metricas_sin_poda);

        std::cout << "[BT referencia] con poda: "
                  << metricas_con_poda.soluciones_encontradas << " soluciones, "
                  << metricas_con_poda.nodos_visitados << " nodos visitados\n";
        std::cout << "[BT referencia] sin poda: "
                  << metricas_sin_poda.soluciones_encontradas << " soluciones, "
                  << metricas_sin_poda.nodos_visitados << " nodos visitados\n";

        if (metricas_con_poda.soluciones_encontradas !=
            metricas_sin_poda.soluciones_encontradas) {
            std::cerr << "[FALLO] El numero de soluciones con poda y sin poda "
                         "no coincide para la instancia de referencia BT.\n";
            fallos++;
        } else {
            std::cout << "[OK] BT: con poda y sin poda coinciden en soluciones.\n";
        }

        if (metricas_con_poda.nodos_visitados >= metricas_sin_poda.nodos_visitados) {
            std::cerr << "[FALLO] La version con poda no visito menos nodos "
                         "que la version sin poda; revisar la poda.\n";
            fallos++;
        } else {
            std::cout << "[OK] BT: la poda redujo el numero de nodos visitados.\n";
        }
    }

    
    // ------------------------------------------------------------------
    {
        std::string hash_objetivo =
            "8d51feb34e3e69f6fa6dffc577e2c60490cf9a7fcd835f9f6af1505b71d74773";
        std::string alfabeto_A2 = "abcdefghijklmnopqrstuvwxyz0123456789";

        // Verificacion extra: confirmar que sha256_hex("abc12") efectivamente
        // produce el hash esperado (valida tu wrapper de la libreria de hash).
        std::string hash_calculado = sha256_hex("abc12");
        if (hash_calculado != hash_objetivo) {
            std::cerr << "[FALLO] sha256_hex(\"abc12\") no coincide con el hash "
                         "esperado.\n  esperado:  " << hash_objetivo
                      << "\n  obtenido:  " << hash_calculado << "\n";
            fallos++;
        } else {
            std::cout << "[OK] sha256_hex(\"abc12\") coincide con el hash de "
                         "referencia.\n";
        }

        BruteForceResult resultado =
            brute_force_search(hash_objetivo, alfabeto_A2, 5);

        std::cout << "[FB referencia] encontrada=" << resultado.found
                  << " password='" << resultado.password
                  << "' candidatos_evaluados=" << resultado.tested << "\n";

        if (!resultado.found || resultado.password != "abc12") {
            std::cerr << "[FALLO] FB: se esperaba encontrar 'abc12', se obtuvo '"
                      << resultado.password << "' (found=" << resultado.found
                      << ").\n";
            fallos++;
        } else {
            std::cout << "[OK] FB: la instancia de referencia se resolvio "
                         "correctamente.\n";
        }
    }

    if (fallos == 0) {
        std::cout << "\nTodas las verificaciones de referencia pasaron.\n";
        return 0;
    } else {
        std::cout << "\n" << fallos << " verificacion(es) fallaron.\n";
        return 1;
    }
}
