#include "fb_solver.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

static void print_usage() {
    std::cout << "Uso:\n";
    std::cout << "  ./app <target_hash> brute <alphabet> <max_length>\n";
    std::cout << "  ./app <target_hash> dict <dictionary.txt>\n";
    std::cout << "Ejemplos:\n";
    std::cout << "  ./app 5e... brute abcdefghijklmnopqrstuvwxyz 4\n";
    std::cout << "  ./app 5e... dict resources/wordlist.txt\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    const std::string target_hash = argv[1];
    const std::string mode = (argc > 2) ? argv[2] : "brute";

    if (mode == "brute" || mode == "alphabet") {
        const std::string alphabet = (argc > 3) ? argv[3] : "abcdefghijklmnopqrstuvwxyz";
        const std::size_t max_length = (argc > 4) ? static_cast<std::size_t>(std::stoul(argv[4])) : 4;

        const auto result = brute_force_search(target_hash, alphabet, max_length);
        if (result.found) {
            std::cout << "Encontrada: " << result.password << "\n";
            std::cout << "Pruebas: " << result.tested << "\n";
            return 0;
        }

        std::cout << "No se encontró ninguna contraseña en el rango indicado.\n";
        std::cout << "Pruebas: " << result.tested << "\n";
        return 2;
    }

    if (mode == "dict" || mode == "dictionary") {
        const std::string dictionary_path = (argc > 3) ? argv[3] : "resources/wordlist.txt";
        const auto result = dictionary_attack(target_hash, dictionary_path);
        if (result.found) {
            std::cout << "Encontrada: " << result.password << "\n";
            std::cout << "Pruebas: " << result.tested << "\n";
            return 0;
        }

        std::cout << "No se encontró la contraseña en el diccionario.\n";
        std::cout << "Pruebas: " << result.tested << "\n";
        return 2;
    }

    std::cout << "Modo no soportado.\n";
    print_usage();
    return 1;
}
