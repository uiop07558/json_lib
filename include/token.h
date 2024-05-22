#pragma once

#include <stdint.h>

typedef enum TokenType {
  Empty,
  Colon,
  Comma,
  RightSquareBr,
  RightCurlyBr,
  LeftSquareBr,
  LeftCurlyBr,
  LiteralTrue,
  LiteralFalse,
  LiteralNull,
  String,
  Number
} TokenType;

typedef struct Token {
  char* str;
  uint32_t len;
  TokenType type;
} Token;
