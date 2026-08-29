#ifndef FB_SOLVER_HPP
#define FB_SOLVER_HPP

#include <cstddef>
#include <string>
#include <vector>

struct BruteForceResult {
    bool found = false;
    std::string password;
    std::size_t tested = 0;
    std::size_t max_length = 0;
    std::string alphabet;
};

struct DictionaryAttackResult {
    bool found = false;
    std::string password;
    std::size_t tested = 0;
    std::string dictionary_path;
};

std::string sha256_hex(const std::string& input);

BruteForceResult brute_force_search(const std::string& target_hash,
                                   const std::string& alphabet,
                                   std::size_t max_length);

DictionaryAttackResult dictionary_attack(const std::string& target_hash,
                                       const std::string& dictionary_path);

std::vector<std::string> read_dictionary_file(const std::string& path);

#endif  // FB_SOLVER_HPP
