// src/main.cpp

// Archivo principal del proyecto.
// Aqui se ejecutan las pruebas de los modulos FB y BT y se guardan
// los resultados en archivos CSV.

#include "bt_solver.hpp"
#include "fb_solver.hpp"
#include "metrics_csv.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace {

// Alfabetos usados en el Modulo FB.
const std::string A1 = "abcdefghijklmnopqrstuvwxyz";
const std::string A2 = "abcdefghijklmnopqrstuvwxyz0123456789";

// Alfabeto usado para las contrasenas del Modulo BT.
const std::string ALFABETO_BT =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789"
    "!@#$%";

// Datos de las instancias que vamos a probar en el Modulo FB.
struct InstanciaFB {
    std::string id;
    std::string alfabeto_nombre;
    std::string alfabeto;
    std::size_t n;
    std::string hash_objetivo;
};

const std::vector<InstanciaFB> INSTANCIAS_FB = {
    {"fb_equipo_1", "A1", A1, 4, "bc80a73a3dea2ddab8727869a905c97f1dcfa5551ad6baf6b1559fe692b82706"},
    {"fb_equipo_2", "A2", A2, 4, "e05f99893c671de040a523f3c5ecd841a3ebc83d247aa419972d1c5c221d62e7"},
    {"fb_equipo_3", "A1", A1, 5, "8367e366a3531c44ceb7174ec3cc7e21ae8acf7115a4962e9f43a62b56256535"},
    {"fb_equipo_4", "A2", A2, 5, "797b2c652cf80fe3f39e921958d71535d511514693427d18cab5a1ad7d6239aa"},
    {"fb_equipo_5", "A1", A1, 6, "6e41c76b775d1bebf2159a4b4a6e8a7777b0c3bf8b7fe6220c30a6355652661f"},
};

// Ejemplo que usamos para comprobar que el modulo FB esta funcionando bien.
// "abc12" usando A2 y n=5.
const std::string HASH_REFERENCIA_FB = "8d51feb34e3e69f6fa6dffc577e2c60490cf9a7fcd835f9f6af1505b71d74773";

// Diferentes configuraciones que se prueban en el Modulo BT.
struct VarianteBT {
    std::string id;
    Politica politica;
    bool costosa = false;
};

// Algunas variantes pueden tardar bastante por la cantidad de combinaciones.
// Por eso se pueden activar o desactivar desde esta variable.
constexpr bool EJECUTAR_VARIANTES_COSTOSAS = false;

const std::vector<VarianteBT> VARIANTES_BT = {
    {"bt_i_completa_n8",        Politica{8, 2, 2, 1, 1}, false},
    {"bt_ii_completa_n6",       Politica{6, 2, 2, 1, 1}, false},
    {"bt_iii_completa_n10",     Politica{10, 2, 2, 1, 1}, true},
    {"bt_iv_relajada_n8",       Politica{8, 1, 0, 0, 0}, false},
    {"bt_v_sin_restriccion_n6", Politica{6, 0, 0, 0, 0}, true},
};

// Politica usada como ejemplo para comprobar el funcionamiento del BT.
const Politica POLITICA_REFERENCIA_BT{6, 2, 1, 1, 1};

// Mide cuanto tiempo tarda en ejecutarse una funcion.
double medir_ms(const std::function<void()>& fn) {
    auto inicio = std::chrono::high_resolution_clock::now();

    fn();

    auto fin = std::chrono::high_resolution_clock::now();

    return std::chrono::duration<double, std::milli>(fin - inicio).count();
}

void ejecutar_modulo_fb() {
    std::cout << "\n=== Modulo FB: fuerza bruta vs diccionario ===\n";

    std::vector<RegistroFB> resultados;

    // Primero comprobamos el ejemplo de referencia.
    auto ref = brute_force_search(HASH_REFERENCIA_FB, A2, 5);

    std::cout << "[Referencia] esperado='abc12' obtenido='"
              << (ref.found ? ref.password : "NO ENCONTRADO") << "'\n";

    // Probamos fuerza bruta con cada una de las instancias.
    for (const auto& inst : INSTANCIAS_FB) {
        BruteForceResult resultado;

        double ms = medir_ms([&]() {
            resultado = brute_force_search(inst.hash_objetivo, inst.alfabeto, inst.n);
        });

        std::cout << "[FB][fuerza_bruta] " << inst.id
                  << " n=" << inst.n << " " << inst.alfabeto_nombre
                  << " -> " << (resultado.found ? resultado.password : "no encontrada")
                  << " (" << ms << " ms, " << resultado.tested << " candidatos)\n";

        RegistroFB r;
        r.instancia = inst.id;
        r.estrategia = "fuerza_bruta";
        r.alfabeto = inst.alfabeto_nombre;
        r.n = static_cast<int>(inst.n);
        r.tiempo_ms = ms;
        r.candidatos_evaluados = static_cast<long long>(resultado.tested);
        r.encontrada = resultado.found;

        resultados.push_back(r);
    }

    // Ahora probamos el ataque por diccionario con las mismas instancias.
    const std::string RUTA_DICCIONARIO = "resources/diccionario.txt";

    for (const auto& inst : INSTANCIAS_FB) {
        DictionaryAttackResult resultado;

        double ms = medir_ms([&]() {
            resultado = dictionary_attack(inst.hash_objetivo, RUTA_DICCIONARIO);
        });

        std::cout << "[FB][diccionario]  " << inst.id
                  << " -> " << (resultado.found ? resultado.password : "no encontrada en diccionario")
                  << " (" << ms << " ms, " << resultado.tested << " palabras probadas)\n";

        RegistroFB r;
        r.instancia = inst.id;
        r.estrategia = "diccionario";
        r.alfabeto = "-";
        r.n = static_cast<int>(inst.n);
        r.tiempo_ms = ms;
        r.candidatos_evaluados = static_cast<long long>(resultado.tested);
        r.encontrada = resultado.found;
        r.en_diccionario = resultado.found;

        resultados.push_back(r);
    }

    exportarCSV_FB(resultados, "results/fb_resultados.csv");
}

void ejecutar_modulo_bt() {
    std::cout << "\n=== Modulo BT: con poda vs sin poda ===\n";

    std::vector<RegistroBT> resultados;

    // Comprobamos primero la politica de referencia.
    {
        MetricasBT metricas_ref;

        backtrackingPoda(
            EstadoParcial{},
            POLITICA_REFERENCIA_BT,
            ALFABETO_BT,
            metricas_ref
        );

        std::cout << "[Referencia] soluciones encontradas (n=6, politica comun) = "
                  << metricas_ref.soluciones_encontradas << "\n";
    }

    for (const auto& variante : VARIANTES_BT) {

        // Saltamos las variantes costosas mientras estamos haciendo pruebas.
        if (variante.costosa && !EJECUTAR_VARIANTES_COSTOSAS) {
            std::cout << "[BT] " << variante.id
                      << " -> SALTADA (costosa=true, EJECUTAR_VARIANTES_COSTOSAS=false)."
                      << " Pon el flag en true para la corrida final.\n";
            continue;
        }

        // Probamos primero la version sin poda.
        MetricasBT metricas_sp;

        double ms_sp = medir_ms([&]() {
            backtrackingSinPoda(
                EstadoParcial{},
                variante.politica,
                ALFABETO_BT,
                metricas_sp
            );
        });

        std::cout << "[BT][sin_poda] " << variante.id
                  << " nodos=" << metricas_sp.nodos_visitados
                  << " soluciones=" << metricas_sp.soluciones_encontradas
                  << " (" << ms_sp << " ms)\n";

        RegistroBT r_sp;
        r_sp.instancia = variante.id;
        r_sp.version = "sin_poda";
        r_sp.n = variante.politica.n;
        r_sp.tiempo_ms = ms_sp;
        r_sp.nodos_generados = metricas_sp.nodos_visitados;
        r_sp.soluciones_encontradas = metricas_sp.soluciones_encontradas;

        resultados.push_back(r_sp);

        // Luego hacemos la misma prueba pero usando poda.
        MetricasBT metricas_cp;

        double ms_cp = medir_ms([&]() {
            backtrackingPoda(
                EstadoParcial{},
                variante.politica,
                ALFABETO_BT,
                metricas_cp
            );
        });

        std::cout << "[BT][con_poda] " << variante.id
                  << " nodos=" << metricas_cp.nodos_visitados
                  << " soluciones=" << metricas_cp.soluciones_encontradas
                  << " (" << ms_cp << " ms)\n";

        RegistroBT r_cp;
        r_cp.instancia = variante.id;
        r_cp.version = "con_poda";
        r_cp.n = variante.politica.n;
        r_cp.tiempo_ms = ms_cp;
        r_cp.nodos_generados = metricas_cp.nodos_visitados;
        r_cp.soluciones_encontradas = metricas_cp.soluciones_encontradas;

        resultados.push_back(r_cp);

        // Las dos versiones deberian encontrar la misma cantidad de soluciones.
        if (metricas_sp.soluciones_encontradas != metricas_cp.soluciones_encontradas) {
            std::cout << "  [ALERTA] con_poda y sin_poda no coinciden en soluciones!\n";
        }
    }

    exportarCSV_BT(resultados, "results/bt_resultados.csv");
}

}  // namespace

int main() {
    ejecutar_modulo_fb();
    ejecutar_modulo_bt();

    std::cout << "\nListo. Revisa results/fb_resultados.csv y results/bt_resultados.csv\n";

    return 0;
}