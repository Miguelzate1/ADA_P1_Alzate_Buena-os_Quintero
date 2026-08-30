// resources/verificar_semilla.cpp
//
// Este programa sirve para verificar los datos de la Seccion 9.
// A partir de los apellidos del equipo calcula:
// 1. La semilla oficial.
// 2. Los parametros de las contrasenas del Modulo BT.
// 3. Las contrasenas del Modulo FB y sus respectivos hashes.
//
// Para compilar desde la raiz del proyecto:
// g++ -std=c++17 -O2 -o verificar_semilla resources/verificar_semilla.cpp
//
// Para ejecutarlo:
// ./verificar_semilla
//
// Nota sobre la ñ:
// El enunciado dice que se deben quitar las tildes, pero no especifica
// que hacer con la ñ. En este programa se mantiene tal como esta.
// Si el programa oficial del curso da un resultado diferente, habria
// que revisar como esta manejando el profesor este caracter.

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// Libreria usada para calcular SHA-256.
// El archivo ya debe estar dentro del proyecto.
#include "../src/third_party/picosha2.h"

namespace {

struct InstanciaFB {
    int numero;
    std::size_t longitud;
    std::string alfabeto;
    std::string nombre_alfabeto;
};

// Alfabetos que se usan en las instancias del Modulo FB.
const std::string A1 = "abcdefghijklmnopqrstuvwxyz";               // 26 caracteres
const std::string A2 = "abcdefghijklmnopqrstuvwxyz0123456789";     // 36 caracteres

// Las instancias empiezan alternando desde A1, como se indica en el enunciado.
const std::vector<InstanciaFB> INSTANCIAS_FB = {
    {1, 4, A1, "A1"},
    {2, 4, A2, "A2"},
    {3, 5, A1, "A1"},
    {4, 5, A2, "A2"},
    {5, 6, A1, "A1"},
};

// Calcula el siguiente numero del generador congruencial lineal.
uint64_t siguiente(uint64_t x) {
    return (1103515245ULL * x + 12345ULL) % (1ULL << 31);
}

// Convierte el texto a su hash SHA-256.
std::string sha256_hex(const std::string& texto) {
    return picosha2::hash256_hex_string(texto);
}

}  // namespace

int main() {
    
    // Cambiar solamente esta parte por los apellidos del equipo
    std::vector<std::string> apellidos = {"alzate", "buenaños", "quintero"};

    // Se ordenan los apellidos antes de hacer la operacion.
    std::sort(apellidos.begin(), apellidos.end());

    // Se unen todos los apellidos en una sola cadena.
    std::string cadena;
    for (const auto& apellido : apellidos) cadena += apellido;

    // Sumamos los valores de los caracteres de la cadena.
    long long suma = 0;
    for (unsigned char c : cadena) suma += static_cast<long long>(c);

    // La semilla se obtiene tomando la suma modulo 100000.
    long long semilla = suma % 100000;

    std::cout << "=== Semilla oficial ===\n";
    std::cout << "Apellidos ordenados: ";
    for (const auto& a : apellidos) std::cout << a << " ";
    std::cout << "\nCadena concatenada : " << cadena << "\n";
    std::cout << "Suma de codigos    : " << suma << "\n";
    std::cout << "Semilla            : " << semilla << "\n\n";

    // Parametros de las contrasenas del Modulo BT.
    int minLower = 2 + static_cast<int>(semilla % 3);
    int minUpper = 1 + static_cast<int>(semilla % 2);
    int minDigit = 1 + static_cast<int>(semilla % 3);
    int minSymbol = 1;
    int n_bt = 8;

    std::cout << "=== Politica de contrasenas (Modulo BT) ===\n";
    std::cout << "minLower  = " << minLower << "\n";
    std::cout << "minUpper  = " << minUpper << "\n";
    std::cout << "minDigit  = " << minDigit << "\n";
    std::cout << "minSymbol = " << minSymbol << "\n";

    // Revisamos si la suma de los minimos supera la longitud de la contrasena.
    int suma_minimos = minLower + minUpper + minDigit + minSymbol;

    if (suma_minimos > n_bt) {
        int exceso = suma_minimos - n_bt;

        // Si se pasa del limite, se reduce minLower para ajustarlo.
        minLower = std::max(0, minLower - exceso);

        std::cout << "Suma de minimos (" << suma_minimos << ") > n=" << n_bt
                  << " -> se redujo minLower a " << minLower << "\n";
    } else {
        std::cout << "Suma de minimos = " << suma_minimos << " <= n=" << n_bt
                  << " (no requiere ajuste)\n";
    }

    std::cout << "\n";

    // Generamos las contrasenas objetivo del Modulo FB.
    std::cout << "=== Contrasenas objetivo (Modulo FB) ===\n";

    uint64_t x = static_cast<uint64_t>(semilla);

    for (const auto& inst : INSTANCIAS_FB) {
        std::string password;

        // Generamos cada caracter usando el valor anterior del generador.
        for (std::size_t i = 0; i < inst.longitud; ++i) {
            x = siguiente(x);
            password += inst.alfabeto[x % inst.alfabeto.size()];
        }

        // Calculamos el SHA-256 de la contrasena generada.
        std::string hash = sha256_hex(password);

        std::cout << "Instancia " << inst.numero
                  << " | n=" << inst.longitud
                  << " | " << inst.nombre_alfabeto
                  << " | password=" << password
                  << " | sha256=" << hash << "\n";
    }

    // Comprobamos tambien el ejemplo que aparece en el enunciado.
    // Para "abc12" usando A2 y n=5, el hash esperado es:
    // 8d51feb34e3e69f6fa6dffc577e2c60490cf9a7fcd835f9f6af1505b71d74773
    //
    // El SHA-256 siempre tiene 64 caracteres hexadecimales.
    // Si en el PDF aparece con 65, probablemente es un error.

    std::string ref = "abc12";

    std::cout << "\n=== Verificacion del ejemplo del enunciado ===\n";
    std::cout << "sha256(\"" << ref << "\") = " << sha256_hex(ref) << "\n";

    return 0;
}