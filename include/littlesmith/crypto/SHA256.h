//
// Created by stefan on 04.03.25.
//

#pragma once

#include <cstring>
#include <fstream>
#include "../util/Bitwise.h"

#include <string>
#include <cstdint>
#include <vector>
#include <iomanip>

namespace littlesmith {

    class SHA256 {
    public:
        static const unsigned int DIGEST_SIZE = (256 / 8);

        void init();

        void update(const std::string& message) { update(reinterpret_cast<const uint8_t*>(message.data()), message.length()); }

        void update(const uint8_t *message, unsigned int len);

        std::vector<uint8_t> final();

        void final(uint8_t *digest);

        static std::string hashString(const std::string &input);
    private:

        const static uint32_t sha256_k[];
        static const unsigned int SHA224_256_BLOCK_SIZE = (512 / 8);
        static void unpack(unsigned int x, uint8_t *str);
        static void pack(const uint8_t *str, unsigned int *x);
        void transform(const uint8_t *message, unsigned int block_nb);

        unsigned int m_tot_len;
        unsigned int m_len;
        uint8_t m_block[2 * SHA224_256_BLOCK_SIZE];
        uint32_t m_h[8];

        static uint32_t f1(uint32_t x) { return (rotate_right(x,  2) ^ rotate_right(x, 13) ^ rotate_right(x, 22)); }
        static uint32_t f2(uint32_t x) { return (rotate_right(x,  6) ^ rotate_right(x, 11) ^ rotate_right(x, 25)); }
        static uint32_t f3(uint32_t x) { return (rotate_right(x,  7) ^ rotate_right(x, 18) ^ shift_right(x,  3)); }
        static uint32_t f4(uint32_t x) { return (rotate_right(x, 17) ^ rotate_right(x, 19) ^ shift_right(x, 10)); }
    };

    void SHA256::unpack(unsigned int x, uint8_t *str) {
        *((str) + 3) = (uint8_t) ((x));
        *((str) + 2) = (uint8_t) ((x) >> 8);
        *((str) + 1) = (uint8_t) ((x) >> 16);
        *((str) + 0) = (uint8_t) ((x) >> 24);
    }

    void SHA256::pack(const uint8_t *str, unsigned int *x) {
        *(x) = ((uint32_t) *((str) + 3))
               | ((uint32_t) *((str) + 2) << 8)
               | ((uint32_t) *((str) + 1) << 16)
               | ((uint32_t) *((str) + 0) << 24);
    }

    const unsigned int SHA256::sha256_k[64] = //UL = uint32_t
            {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
             0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
             0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
             0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
             0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
             0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
             0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
             0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
             0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
             0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
             0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
             0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
             0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
             0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
             0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
             0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

    void SHA256::transform(const uint8_t *message, unsigned int block_nb) {
        uint32_t w[64];
        uint32_t wv[8];
        uint32_t t1, t2;
        const uint8_t *sub_block;
        int i;
        int j;
        for (i = 0; i < (int) block_nb; i++) {
            sub_block = message + (i << 6);
            for (j = 0; j < 16; j++) {
                pack(&sub_block[j << 2], &w[j]);
            }
            for (j = 16; j < 64; j++) {
                w[j] = f4(w[j - 2]) + w[j - 7] + f3(w[j - 15]) + w[j - 16];
            }
            for (j = 0; j < 8; j++) {
                wv[j] = m_h[j];
            }
            for (j = 0; j < 64; j++) {
                t1 = wv[7] + f2(wv[4]) + choose(wv[4], wv[5], wv[6])
                     + sha256_k[j] + w[j];
                t2 = f1(wv[0]) + majority(wv[0], wv[1], wv[2]);
                wv[7] = wv[6];
                wv[6] = wv[5];
                wv[5] = wv[4];
                wv[4] = wv[3] + t1;
                wv[3] = wv[2];
                wv[2] = wv[1];
                wv[1] = wv[0];
                wv[0] = t1 + t2;
            }
            for (j = 0; j < 8; j++) {
                m_h[j] += wv[j];
            }
        }
    }

    void SHA256::init() {
        m_h[0] = 0x6a09e667;
        m_h[1] = 0xbb67ae85;
        m_h[2] = 0x3c6ef372;
        m_h[3] = 0xa54ff53a;
        m_h[4] = 0x510e527f;
        m_h[5] = 0x9b05688c;
        m_h[6] = 0x1f83d9ab;
        m_h[7] = 0x5be0cd19;
        m_len = 0;
        m_tot_len = 0;
    }

    void SHA256::update(const uint8_t *message, unsigned int len) {
        unsigned int block_nb;
        unsigned int new_len, rem_len, tmp_len;
        const uint8_t *shifted_message;
        tmp_len = SHA224_256_BLOCK_SIZE - m_len;
        rem_len = len < tmp_len ? len : tmp_len;
        memcpy(&m_block[m_len], message, rem_len);
        if (m_len + len < SHA224_256_BLOCK_SIZE) {
            m_len += len;
            return;
        }
        new_len = len - rem_len;
        block_nb = new_len / SHA224_256_BLOCK_SIZE;
        shifted_message = message + rem_len;
        transform(m_block, 1);
        transform(shifted_message, block_nb);
        rem_len = new_len % SHA224_256_BLOCK_SIZE;
        memcpy(m_block, &shifted_message[block_nb << 6], rem_len);
        m_len = rem_len;
        m_tot_len += (block_nb + 1) << 6;
    }

    void SHA256::final(uint8_t *digest) {
        unsigned int block_nb;
        unsigned int pm_len;
        unsigned int len_b;
        int i;
        block_nb = (1 + ((SHA224_256_BLOCK_SIZE - 9)
                         < (m_len % SHA224_256_BLOCK_SIZE)));
        len_b = (m_tot_len + m_len) << 3;
        pm_len = block_nb << 6;
        memset(m_block + m_len, 0, pm_len - m_len);
        m_block[m_len] = 0x80;
        unpack(len_b, m_block + pm_len - 4);
        transform(m_block, block_nb);
        for (i = 0; i < 8; i++) {
            unpack(m_h[i], &digest[i << 2]);
        }
    }

    std::vector<uint8_t> SHA256::final() {
        auto buffer = std::vector<uint8_t>(SHA256::DIGEST_SIZE, 0);
        final(buffer.data());
        return buffer;
    }

    std::string SHA256::hashString(const std::string &input) {
        SHA256 ctx{};
        ctx.init();
        ctx.update(input);
        auto digest = ctx.final();

        std::stringstream ss;
        for (const auto &byte: digest) {
            ss << std::setw(2) << std::setfill('0') << byte;
        }
        return {ss.str()};
    }
}