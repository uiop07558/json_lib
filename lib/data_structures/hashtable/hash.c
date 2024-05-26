// implementation of xxhash3

// xxHash Library
// Copyright (c) 2012-2021 Yann Collet
// All rights reserved.
//
// BSD 2-Clause License (https://www.opensource.org/licenses/bsd-license.php)
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or
//   other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <byteswap.h>
#include "hash.h"

#define _PRIME32_1 0x9E3779B1U
#define _PRIME32_2 0x85EBCA77U 
#define _PRIME32_3 0xC2B2AE3DU
#define _PRIME64_1 0x9E3779B185EBCA87ULL
#define _PRIME64_2 0xC2B2AE3D27D4EB4FULL
#define _PRIME64_3 0x165667B19E3779F9ULL
#define _PRIME64_4 0x85EBCA77C2B2AE63ULL
#define _PRIME64_5 0x27D4EB2F165667C5ULL
#define _PRIME_MX1 0x165667919E3779F9ULL
#define _PRIME_MX2 0x9FB21C651E98DF25ULL

#define _SECRET_LENGTH 192
#define _STRIPES_PER_BLOCK (_SECRET_LENGTH - 64) / 8
#define _BLOCKSIZE 64 * _STRIPES_PER_BLOCK

static inline uint64_t _xxAval(uint64_t x) {
  x ^= (x >> 37);
  x *= _PRIME_MX1;
  x ^= (x >> 32);
  return x;
}

static inline uint64_t _xxAval64(uint64_t x) {
  x ^= (x >> 33);
  x *= _PRIME64_2;
  x ^= (x >> 29);
  x *= _PRIME64_3;
  x ^= (x >> 32);
  return x;
}

static inline uint64_t _leftRotate64(uint64_t x, uint64_t s) {
  return (x << s) | (x >> (64 - s));
}

static inline uint64_t _xxMixStep(uint8_t* data, uint8_t* secret, uint64_t seed) {
  uint64_t dataWords[2];
  dataWords[0] = ((uint64_t*) data)[0];
  dataWords[1] = ((uint64_t*) data)[1];
  uint64_t secretWords[2];
  secretWords[0] = ((uint64_t*) secret)[0];
  secretWords[1] = ((uint64_t*) secret)[1];
  __uint128_t mul = (__uint128_t) (dataWords[0] ^ (secretWords[0] + seed)) * (__uint128_t) (dataWords[1] ^ (secretWords[1] - seed));
  uint64_t* mul64 = (uint64_t*) &mul;
  return mul64[0] ^ mul64[1];
}

static inline void _accumulate(uint64_t acc[8], uint64_t stripe[8], uint8_t* secret) {
  for (size_t i = 0; i < 8; i++) {
    uint64_t value = stripe[i] ^ ((uint64_t*) secret)[i];
    acc[i ^ 1] += stripe[i];
    acc[i] += (value >> 32) * (value & 0xFFFFFFFFULL); // highHalf * lowHalf
  }
}

static inline void _round(uint64_t acc[8], uint8_t* block, uint8_t* secret) {
  for (size_t i = 0; i < _STRIPES_PER_BLOCK; i++) {
    uint64_t* stripe = (uint64_t*) (block + i*64);
    _accumulate(acc, stripe, secret + i*8);
  }
  for (size_t i = 0; i < 8; i++) {
    acc[i] ^= acc[i] >> 47;
    acc[i] ^= ((uint64_t*) (secret + (_SECRET_LENGTH - 64)))[i];
    acc[i] *= _PRIME32_1;
  }
}

uint64_t ds_h_xxHash364(uint8_t* input, size_t length) {
  static uint64_t seed = 0;
  static uint8_t secret[192] = {
    0xb8, 0xfe, 0x6c, 0x39, 0x23, 0xa4, 0x4b, 0xbe, 0x7c, 0x01, 0x81, 0x2c, 0xf7, 0x21, 0xad, 0x1c,
    0xde, 0xd4, 0x6d, 0xe9, 0x83, 0x90, 0x97, 0xdb, 0x72, 0x40, 0xa4, 0xa4, 0xb7, 0xb3, 0x67, 0x1f,
    0xcb, 0x79, 0xe6, 0x4e, 0xcc, 0xc0, 0xe5, 0x78, 0x82, 0x5a, 0xd0, 0x7d, 0xcc, 0xff, 0x72, 0x21,
    0xb8, 0x08, 0x46, 0x74, 0xf7, 0x43, 0x24, 0x8e, 0xe0, 0x35, 0x90, 0xe6, 0x81, 0x3a, 0x26, 0x4c,
    0x3c, 0x28, 0x52, 0xbb, 0x91, 0xc3, 0x00, 0xcb, 0x88, 0xd0, 0x65, 0x8b, 0x1b, 0x53, 0x2e, 0xa3,
    0x71, 0x64, 0x48, 0x97, 0xa2, 0x0d, 0xf9, 0x4e, 0x38, 0x19, 0xef, 0x46, 0xa9, 0xde, 0xac, 0xd8,
    0xa8, 0xfa, 0x76, 0x3f, 0xe3, 0x9c, 0x34, 0x3f, 0xf9, 0xdc, 0xbb, 0xc7, 0xc7, 0x0b, 0x4f, 0x1d,
    0x8a, 0x51, 0xe0, 0x4b, 0xcd, 0xb4, 0x59, 0x31, 0xc8, 0x9f, 0x7e, 0xc9, 0xd9, 0x78, 0x73, 0x64,
    0xea, 0xc5, 0xac, 0x83, 0x34, 0xd3, 0xeb, 0xc3, 0xc5, 0x81, 0xa0, 0xff, 0xfa, 0x13, 0x63, 0xeb,
    0x17, 0x0d, 0xdd, 0x51, 0xb7, 0xf0, 0xda, 0x49, 0xd3, 0x16, 0x55, 0x26, 0x29, 0xd4, 0x68, 0x9e,
    0x2b, 0x16, 0xbe, 0x58, 0x7d, 0x47, 0xa1, 0xfc, 0x8f, 0xf8, 0xb8, 0xd1, 0x7a, 0xd0, 0x31, 0xce,
    0x45, 0xcb, 0x3a, 0x8f, 0x95, 0x16, 0x04, 0x28, 0xaf, 0xd7, 0xfb, 0xca, 0xbb, 0x4b, 0x40, 0x7e,
  };

  if (length <= 3) {
    uint32_t combined = (uint32_t) input[length - 1] | ((uint32_t) length << 8) | ((uint32_t) input[0] << 16) | ((uint32_t) input[length >> 1] << 24);
    uint32_t* words = (uint32_t*) secret;
    uint64_t value = ((uint64_t) (words[0] ^ words[1]) + seed) ^ ((uint64_t) combined);
    return _xxAval64(value);
  }
  else if (length >= 4 && length <= 8) {
    uint32_t inputFirst = ((uint32_t*) input)[0];
    uint32_t inputLast = 0;
    for (size_t i = 4; i < length; i++) {
      inputLast |= ((uint32_t) input[i] << (8*(i-4)));
    }
    uint64_t modSeed = seed ^ ((uint64_t) bswap_64(seed & 0xFFFFFFFFULL));

    uint64_t words[2]; 
    words[0] = ((uint64_t*) secret)[1];
    words[1] = ((uint64_t*) secret)[2];
    uint64_t combined = (uint64_t) inputLast | ((uint64_t) inputFirst << 32);
    uint64_t value = ((words[0] ^ words[1]) - modSeed) ^ combined;
    value ^= _leftRotate64(value, 49);
    value ^= _leftRotate64(value, 24);
    value *= _PRIME_MX2;
    value ^= (value >> 35) + length;
    value *= _PRIME_MX2;
    value ^= value >> 28;
    return value;
  }
  else if (length >= 9 && length <= 16) {
    uint64_t inputFirst = ((uint64_t*) input)[0];
    uint64_t inputLast = 0;
    for (size_t i = 8; i < length; i++) {
      inputLast |= ((uint64_t) input[i] << (8*(i-4)));
    }

    uint64_t words[4];
    words[0] = ((uint64_t*) secret)[3];
    words[1] = ((uint64_t*) secret)[4];
    words[2] = ((uint64_t*) secret)[5];
    words[3] = ((uint64_t*) secret)[6];

    uint64_t low = ((words[0] ^ words[1]) + seed) ^ inputFirst;
    uint64_t high = ((words[2] ^ words[3]) - seed) ^ inputLast;
    __uint128_t mul = (__uint128_t) low * (__uint128_t) high;
    uint64_t* mul64 = (uint64_t*) &mul;
    uint64_t value = bswap_64(low) + high + (mul64[0] ^ mul64[1]) + length;
    return _xxAval(value);
  }
  else if (length >= 17 && length <= 240) {
    uint64_t acc = length * _PRIME64_1;
    if (length <= 128) {
      uint64_t rounds = ((length - 1) >> 5) + 1;
      for (int64_t i = rounds - 1; i >= 0; i--) {
        acc += _xxMixStep(input + i*16, secret + i*32, seed);
        acc += _xxMixStep(input + (length - i*16 - 16), secret + i*32+16, seed);
      }
    }
    else {
      uint64_t numChunks = length >> 4;
      for (uint64_t i = 0; i < 8; i++) {
        acc += _xxMixStep(input + i*16, secret + i*16, seed);
      }
      acc = _xxAval(acc);
      for (uint64_t i = 8; i < numChunks; i++) {
        acc += _xxMixStep(input + i*16, secret + ((i-8)*16 + 3), seed);
      }
      acc += _xxMixStep(input + (length - 16), secret + 119, seed);
    }
    
    return _xxAval(acc);
  }
  else if (length > 240) {
    uint64_t acc[8] = {
      _PRIME32_3, _PRIME64_1, _PRIME64_2, _PRIME64_3,
      _PRIME64_4, _PRIME32_2, _PRIME64_5, _PRIME32_1
    };

    size_t remainingLength = length;
    size_t curBlock = 0;
    while (remainingLength > _BLOCKSIZE) {
      uint8_t* block = input + (_BLOCKSIZE * curBlock);
      _round(acc, block, secret);
      curBlock++;
      remainingLength -= _BLOCKSIZE;
    }
    
    uint8_t* block = input + (_BLOCKSIZE * curBlock);
    size_t fullStripes = (remainingLength - 1) / 64;
    for (size_t i = 0; i < fullStripes; i++) {
      uint64_t* stripe = (uint64_t*) (block + i*64);
      _accumulate(acc, stripe, secret + i*8);
    }
    uint64_t* lastStripe = (uint64_t*) (input + (length - 64));
    _accumulate(acc, lastStripe, secret + (_SECRET_LENGTH - 71));

    uint64_t* secretWords = (uint64_t*) (secret + 11);
    uint64_t result = (uint64_t) length * _PRIME64_1;
    for (size_t i = 0; i < 4; i++) {
      __uint128_t mul = (acc[i*2] ^ secretWords[i*2]) * (acc[i*2 + 1] ^ secretWords[i*2 + + 1]);
      uint64_t* mul64 = (uint64_t*) &mul;
      result += mul64[0] ^ mul64[1];
    }
    return _xxAval(result);
  }
}