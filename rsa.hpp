#ifndef RSA_PRIMES_HPP
#define RSA_PRIMES_HPP

#include <cstdint>
#include <iostream>
#include <tuple>
#include <random>
#include "blockchain.h"

namespace rsa
{
    inline bool isPrime(uint32_t n)
    {
        if (n < 2) return false;
        if (n == 2 || n == 3) return true;
        if (n % 2 == 0) return false;
        for (uint32_t i = 3; i * i <= n; i += 2)
            if (n % i == 0) return false;
        return true;
    }

    inline uint32_t generatePrime(uint32_t min = 1000000000, uint32_t max = 0xFFFFFFFF)
    {
        static std::mt19937_64 rng(std::random_device{}());
        std::uniform_int_distribution<uint32_t> dist(min, max);

        uint32_t p;
        do {
            p = dist(rng);
            if (p % 2 == 0) p++;
        } while (!isPrime(p));
        return p;
    }

    inline void generateTwoPrimes(uint32_t &p, uint32_t &q)
    {
        p = generatePrime();
        do { q = generatePrime(); } while (q == p);
    }

    inline uint64_t phi(uint32_t p, uint32_t q)
    {
        return static_cast<uint64_t>(p - 1) * (q - 1);
    }

    inline uint64_t gcd(uint64_t a, uint64_t b)
    {
        while (b != 0)
        {
            uint64_t r = a % b;
            a = b;
            b = r;
        }
        return a;
    }

    inline uint64_t modInverse(uint64_t e, uint64_t phi)
    {
        int64_t t = 0, newt = 1;
        int64_t r = phi, newr = e;

        while (newr != 0)
        {
            uint64_t q = r / newr;
            std::tie(t, newt) = std::make_tuple(newt, t - q * newt);
            std::tie(r, newr) = std::make_tuple(newr, r - q * newr);
        }

        if (r > 1) return 0; // not invertible
        if (t < 0) t += phi;
        return t;
    }

    inline uint64_t getPublicKey(uint64_t phi)
    {
        uint64_t e = generatePrime();
        while (gcd(phi, e) != 1 || e >= phi)
        {
            e = generatePrime();
        }
        return e;
    }

    inline void generate_keys(Wallet &w)
    {
        uint32_t p, q;
        generateTwoPrimes(p, q);
        uint64_t eulertot = phi(p, q);

        uint64_t e, d;
        do {
            e = getPublicKey(eulertot);
            d = modInverse(e, eulertot);
        } while (d == 0);

        w.publicKey = e;
        w.privateKey = d;
        w.modulus = static_cast<uint64_t>(p) * q;
    }
}
#endif
