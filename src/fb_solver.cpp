#include "fb_solver.hpp"

#include "third_party/picosha2.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string trim(const std::string& value) {
    std::size_t first = 0;
    while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first]))) {
        ++first;
    }

    std::size_t last = value.size();
    while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1]))) {
        --last;
    }

    return value.substr(first, last - first);
}

void generate_candidates(const std::string& alphabet,
                         std::size_t current_length,
                         std::size_t max_length,
                         std::string prefix,
                         const std::function<void(const std::string&)>& callback) {
    if (current_length == max_length) {
        callback(prefix);
        return;
    }

    for (char ch : alphabet) {
        generate_candidates(alphabet, current_length + 1, max_length, prefix + ch, callback);
    }
}

}  // namespace

std::string sha256_hex(const std::string& input) {
    return picosha2::hash256_hex_string(input);
}

BruteForceResult brute_force_search(const std::string& target_hash,
                                   const std::string& alphabet,
                                   std::size_t max_length) {
    BruteForceResult result;
    result.alphabet = alphabet;
    result.max_length = max_length;

    if (max_length == 0 || alphabet.empty()) {
        return result;
    }

    for (std::size_t length = 1; length <= max_length; ++length) {
        std::function<void(const std::string&)> callback = [&](const std::string& candidate) {
            ++result.tested;
            if (sha256_hex(candidate) == target_hash) {
                result.found = true;
                result.password = candidate;
                return;
            }
        };

        generate_candidates(alphabet, 0, length, "", callback);

        if (result.found) {
            break;
        }
    }

    return result;
}

DictionaryAttackResult dictionary_attack(const std::string& target_hash,
                                       const std::string& dictionary_path) {
    DictionaryAttackResult result;
    result.dictionary_path = dictionary_path;

    std::ifstream file(dictionary_path);
    if (!file.is_open()) {
        return result;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string candidate = trim(line);
        if (candidate.empty()) {
            continue;
        }

        ++result.tested;
        if (sha256_hex(candidate) == target_hash) {
            result.found = true;
            result.password = candidate;
            return result;
        }
    }

    return result;
}

std::vector<std::string> read_dictionary_file(const std::string& path) {
    std::vector<std::string> words;
    std::ifstream file(path);
    if (!file.is_open()) {
        return words;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string candidate = trim(line);
        if (!candidate.empty()) {
            words.push_back(candidate);
        }
    }

    return words;
}
