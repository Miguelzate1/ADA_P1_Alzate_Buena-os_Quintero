#ifndef PICOSHA2_H
#define PICOSHA2_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace picosha2 {
namespace detail {

inline std::uint32_t rotr(std::uint32_t x, std::size_t n) {
    return (x >> n) | (x << (32u - n));
}

inline std::uint32_t ch(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
    return (x & y) ^ (~x & z);
}

inline std::uint32_t maj(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

inline std::uint32_t sigma0(std::uint32_t x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

inline std::uint32_t sigma1(std::uint32_t x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

inline std::uint32_t gamma0(std::uint32_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

inline std::uint32_t gamma1(std::uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

constexpr std::array<std::uint32_t, 64> k = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu, 0x59f111f1u,
    0x923f82a4u, 0xab1c5ed5u, 0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u,
    0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u, 0xe49b69c1u, 0xefbe4786u,
    0x0fc19dc6u, 0x240ca1ccu, 0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
    0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u,
    0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u,
    0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u, 0xa2bfe8a1u, 0xa81a664bu,
    0xc24b8b70u, 0xc76c51a3u, 0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
    0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au,
    0x5b9cca4fu, 0x682e6ff3u, 0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u
};

inline void process_block(const std::uint8_t* block, std::array<std::uint32_t, 8>& state) {
    std::array<std::uint32_t, 64> w{};
    for (std::size_t i = 0; i < 16; ++i) {
        const std::size_t offset = i * 4;
        w[i] = (static_cast<std::uint32_t>(block[offset]) << 24) |
               (static_cast<std::uint32_t>(block[offset + 1]) << 16) |
               (static_cast<std::uint32_t>(block[offset + 2]) << 8) |
               static_cast<std::uint32_t>(block[offset + 3]);
    }

    for (std::size_t i = 16; i < 64; ++i) {
        w[i] = gamma1(w[i - 2]) + w[i - 7] + gamma0(w[i - 15]) + w[i - 16];
    }

    std::uint32_t a = state[0];
    std::uint32_t b = state[1];
    std::uint32_t c = state[2];
    std::uint32_t d = state[3];
    std::uint32_t e = state[4];
    std::uint32_t f = state[5];
    std::uint32_t g = state[6];
    std::uint32_t h = state[7];

    for (std::size_t i = 0; i < 64; ++i) {
        const std::uint32_t t1 = h + sigma1(e) + ch(e, f, g) + k[i] + w[i];
        const std::uint32_t t2 = sigma0(a) + maj(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}

inline std::array<std::uint8_t, 32> hash256(const std::uint8_t* data, std::size_t len) {
    std::array<std::uint32_t, 8> state = {
        0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u, 0xa54ff53au,
        0x510e527fu, 0x9b05688cu, 0x1f83d9abu, 0x5be0cd19u
    };

    std::vector<std::uint8_t> padded(data, data + len);
    padded.push_back(0x80u);

    while ((padded.size() % 64u) != 56u) {
        padded.push_back(0x00u);
    }

    const std::uint64_t bit_length = static_cast<std::uint64_t>(len) * 8u;
    for (int i = 7; i >= 0; --i) {
        padded.push_back(static_cast<std::uint8_t>((bit_length >> (8u * i)) & 0xffu));
    }

    for (std::size_t i = 0; i + 64 <= padded.size(); i += 64) {
        process_block(padded.data() + i, state);
    }

    std::array<std::uint8_t, 32> digest{};
    for (std::size_t i = 0; i < state.size(); ++i) {
        digest[i * 4 + 0] = static_cast<std::uint8_t>((state[i] >> 24) & 0xffu);
        digest[i * 4 + 1] = static_cast<std::uint8_t>((state[i] >> 16) & 0xffu);
        digest[i * 4 + 2] = static_cast<std::uint8_t>((state[i] >> 8) & 0xffu);
        digest[i * 4 + 3] = static_cast<std::uint8_t>(state[i] & 0xffu);
    }
    return digest;
}

inline std::string bytes_to_hex(const std::array<std::uint8_t, 32>& digest) {
    static constexpr char hex[] = "0123456789abcdef";
    std::string out;
    out.reserve(64);
    for (std::uint8_t byte : digest) {
        out.push_back(hex[(byte >> 4) & 0x0F]);
        out.push_back(hex[byte & 0x0F]);
    }
    return out;
}

}  // namespace detail

inline std::string hash256_hex_string(const std::string& input) {
    const auto* data = reinterpret_cast<const std::uint8_t*>(input.data());
    auto digest = detail::hash256(data, input.size());
    return detail::bytes_to_hex(digest);
}

inline std::string hash256_hex_string(const std::vector<std::uint8_t>& input) {
    auto digest = detail::hash256(input.data(), input.size());
    return detail::bytes_to_hex(digest);
}

}  // namespace picosha2

#endif  // PICOSHA2_H
