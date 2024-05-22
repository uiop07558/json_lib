#include <string.h>
#include <stdbool.h>
#include "../include/token_vector_def.h"
#include "../include/token.h"

#define IS_WHITESPACE(chr) ((chr == '\t') || (chr == '\r') || (chr == '\n') || (chr == ' '))
#define IS_DIGIT(chr) ((chr == '0') || (chr == '1') || (chr == '2') || (chr == '3') || (chr == '4') || \
                       (chr == '5') || (chr == '6') || (chr == '7') || (chr == '8') || (chr == '9'))

void lexer(char* input, ds_Vector_token* tokenList) {
  size_t len = strlen(input);

  Token curTok = {NULL, 0, Empty};
  size_t i = 0;
  while (i < len) {
    if (input[i] == '[') {
      curTok.type = LeftSquareBr;
    }
    else if (input[i] == ']') {
      curTok.type = RightSquareBr;
    }
    else if (input[i] == '{') {
      curTok.type = LeftCurlyBr;
    }
    else if (input[i] == '}') {
      curTok.type = RightCurlyBr;
    }
    else if (input[i] == ':') {
      curTok.type = Colon;
    }
    else if (input[i] == ',') {
      curTok.type = Comma;
    }
    else if (input[i] == '"' && (i + 1) < len) {
      curTok.type = String;
      i++;
      if (input[i] != '"') {
        curTok.str = input + i;
        size_t escaped = SIZE_MAX;
        while (i < len) {
          if (escaped != i && input[i] == '\\') {
            escaped = i + 1;
          }

          if (escaped != i && input[i] == '"') {
            break;
          }

          curTok.len++;
          i++;
        }
      }
    }
    else if (IS_DIGIT(input[i]) || input[i] == '-') {
      curTok.type = Number;
      curTok.str = input + i;
      curTok.len = 1;
      i++;
      while ((IS_DIGIT(input[i]) || input[i] == '-' || input[i] == '+' || 
      input[i] == '.' || input[i] == 'E' || input[i] == 'e') && i < len) {
        curTok.len++;
        i++;
      }
      i--;
    }
    else if (i + 3 < len && !memcmp(input + i, "null", 4)) {
      curTok.type = LiteralNull;
      i += 3;
    }
    else if (i + 3 < len && !memcmp(input + i, "true", 4)) {
      curTok.type = LiteralTrue;
      i += 3;
    }
    else if (i + 4 < len && !memcmp(input + i, "false", 5)) {
      curTok.type = LiteralFalse;
      i += 4;
    }
    
    if (curTok.type != Empty) {
      ds_vec_appendElement_token(tokenList, curTok);
      curTok.len = 0;
      curTok.str = NULL;
      curTok.type = Empty;
    }
    i++;
  }
}