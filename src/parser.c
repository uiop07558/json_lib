#include "../include/parser.h"

#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "../include/char_vector_def.h"
#include "../include/token_vector_def.h"
#include "../include/value_vector_def.h"
#include "../include/value_map_def.h"
#include "../include/token.h"
#include "../include/status.h"
#include "../include/value.h"
#include "../include/lexer.h"
#include "../include/utils.h"
#include "../include/destructor.h"

#define IS_DIGIT(chr) ((chr == '0') || (chr == '1') || (chr == '2') || (chr == '3') || (chr == '4') || \
                       (chr == '5') || (chr == '6') || (chr == '7') || (chr == '8') || (chr == '9'))

#define IS_VALUE(token) (token->type == Number || token->type == String || token->type == LiteralFalse || \
                         token->type == LiteralTrue || token->type == LiteralNull || token->type == LeftCurlyBr || \
                         token->type == LeftSquareBr)

static inline json_Status parseValue(ds_Vector_token* tokenList, size_t i, json_Value* output, size_t* offset);
static json_Status parseObject(ds_Vector_token* tokenList, size_t start, json_Value* output, size_t* offset);
static json_Status parseArray(ds_Vector_token* tokenList, size_t start, json_Value* output, size_t* offset);

static json_Status parseNumber(Token* token, json_Value* output) {
  json_Status status;
  output->type = json_Number;

  if (token->type != Number) {
    status = json_status_InvalidInput;
    return status;
  }

  bool negative = token->str[0] == '-';
  int64_t flPoint = -1;

  int64_t expPos = -1;
  bool expNeg = false;
  int64_t exponent = 0;

  double number = 0;
  for (int64_t i = (int64_t) negative; i < token->len; i++) {
    char curChr = token->str[i];
    if (curChr == '.' && flPoint == -1 && expPos == -1) {
      flPoint = i;
    }
    else if ((curChr == 'E' || curChr == 'e') && expPos == -1 && (i + 1) < token->len) {
      expPos = i;
    }
    else if (IS_DIGIT(curChr) && (flPoint == -1 && expPos == -1)) {
      number *= 10;
      number += curChr - '0';
    }
    else if (IS_DIGIT(curChr) && (flPoint != -1 && expPos == -1)) {
      number += ((double) (curChr - '0')) * pow(10, flPoint - i);
    }
    else if ((curChr == '+' || curChr == '-') && expPos != -1) {
      expNeg = curChr == '-';
    }
    else if (IS_DIGIT(curChr) && expPos != -1) {
      exponent *= 10;
      exponent += curChr - '0';
    }
    else {
      status = json_status_InvalidInput;
      return status;
    }
  }

  if (negative) {
    number *= -1;
  }

  if (expPos != -1) {
    if (expNeg) {
      exponent *= -1;
    }
    number *= pow(10, exponent);
  }

  status = json_status_OK;
  output->data.number = number;
  return status;
}

static size_t parseUnicodeEsc(Token* token, size_t pos, ds_Vector_char* string) {
  size_t offset = 0;
  if ((pos + 4) >= token->len) {
    return offset;
  }

  uint32_t codepoint = 0;
  bool pair = ((pos + 10) < token->len) && (token->str[pos + 5] == '\\') && (token->str[pos + 6] == 'u');

  if (!pair) {
    offset = 4;

    codepoint += hexChrToInt(token->str[pos + 1]);
    codepoint *= 16;
    codepoint += hexChrToInt(token->str[pos + 2]);
    codepoint *= 16;
    codepoint += hexChrToInt(token->str[pos + 3]);
    codepoint *= 16;
    codepoint += hexChrToInt(token->str[pos + 4]);
  }
  else {
    uint32_t high = 0;
    uint32_t low = 0;
    
    high += hexChrToInt(token->str[pos + 1]);
    high *= 16;
    high += hexChrToInt(token->str[pos + 2]);
    high *= 16;
    high += hexChrToInt(token->str[pos + 3]);
    high *= 16;
    high += hexChrToInt(token->str[pos + 4]);

    low += hexChrToInt(token->str[pos + 7]);
    low *= 16;
    low += hexChrToInt(token->str[pos + 8]);
    low *= 16;
    low += hexChrToInt(token->str[pos + 9]);
    low *= 16;
    low += hexChrToInt(token->str[pos + 10]);

    if (high >= 0xD800 && high <= 0xDBFF && low >= 0xDC00 && low <= 0xDFFF) {
      offset = 10;
      high = (high - 0xD800) * 0x400;
      low = low - 0xDC00;
      codepoint = high + low + 0x10000;
    }
    else {
      offset = 4;
      codepoint = high;
    }
  }

  if (codepoint <= 0x7F) {
    char first = codepoint;
    ds_vec_appendElement_char(string, first);
  }
  else if (codepoint >= 0x80 && codepoint <= 0x7FF) {
    char first = 0b11000000 + (codepoint >> 6);
    char second = 0b10000000 + (codepoint & 0b00111111);
    ds_vec_appendElement_char(string, first);
    ds_vec_appendElement_char(string, second);
  }
  else if (codepoint >= 0x800 && codepoint <= 0xFFFF) {
    char first = 0b11100000 + (codepoint >> 12);
    char second = 0b10000000 + ((codepoint >> 6) & 0b00111111);
    char third = 0b10000000 + (codepoint & 0b00111111);
    ds_vec_appendElement_char(string, first);
    ds_vec_appendElement_char(string, second);
    ds_vec_appendElement_char(string, third);
  }
  else if (codepoint >= 0xFFFF && codepoint <= 0x10FFFF) {
    char first = 0b11110000 + (codepoint >> 18);
    char second = 0b10000000 + ((codepoint >> 12) & 0b00111111);
    char third = 0b10000000 + ((codepoint >> 6) & 0b00111111);
    char fourth = 0b10000000 + (codepoint & 0b00111111);
    ds_vec_appendElement_char(string, first);
    ds_vec_appendElement_char(string, second);
    ds_vec_appendElement_char(string, third);
    ds_vec_appendElement_char(string, fourth);
  }

  return offset;
}

static json_Status parseString(Token* token, json_Value* output) {
  json_Status status;
  output->type = json_String;

  ds_Vector_char string;
  ds_vec_initVector_char(&string, 0);

  size_t escaped = SIZE_MAX;
  size_t i = 0;
  while (i < token->len) {
    char curChr = token->str[i];

    if (escaped == i) {
      if (curChr == '\\') {
        ds_vec_appendElement_char(&string, '\\');
      }
      else if (curChr == '"') {
        ds_vec_appendElement_char(&string, '"');
      }
      else if (curChr == '/') {
        ds_vec_appendElement_char(&string, '/');
      }
      else if (curChr == 'b') {
        ds_vec_appendElement_char(&string, '\b');
      }
      else if (curChr == 'f') {
        ds_vec_appendElement_char(&string, '\f');
      }
      else if (curChr == 'n') {
        ds_vec_appendElement_char(&string, '\n');
      }
      else if (curChr == 'r') {
        ds_vec_appendElement_char(&string, '\r');
      }
      else if (curChr == 't') {
        ds_vec_appendElement_char(&string, '\t');
      }
      else if (curChr == 'u') {
        i += parseUnicodeEsc(token, i, &string);
      }
      
      i++;
      continue;
    }

    if (escaped != i && curChr == '\\') {
      escaped = i + 1;
      i++;
      continue;
    }
    i++;
    ds_vec_appendElement_char(&string, curChr);
  }
  ds_vec_appendElement_char(&string, '\0');

  output->data.string = (char*) malloc(string.len * sizeof(char));
  memcpy(output->data.string, string.buf, string.len);
  ds_vec_destroyVector_char(&string);

  status = json_status_OK;
  return status;
}

static inline json_Status parseValue(ds_Vector_token* tokenList, size_t i, json_Value* output, size_t* offset) {
  Token* token = tokenList->buf + i;
  json_Status status = json_status_OK;
  *offset = 0;
  if (token->type == Number) {
    status = parseNumber(token, output);
  }
  else if (token->type == String) {
    status = parseString(token, output);
  }
  else if (token->type == LiteralFalse) {
    output->type = json_Boolean;
    output->data.boolean = false;
    return json_status_OK;
  }
  else if (token->type == LiteralTrue) {
    output->type = json_Boolean;
    output->data.boolean = true;
    return json_status_OK;
  }
  else if (token->type == LiteralNull) {
    output->type = json_Null;
    output->data.boolean = 0;
    return json_status_OK;
  }
  else if (token->type == LeftSquareBr) {
    status = parseArray(tokenList, i + 1, output, offset);
  }
  else if (token->type == LeftCurlyBr) {
    status = parseObject(tokenList, i + 1, output, offset);
  }
  else {
    return json_status_InvalidInput;
  }

  return status;
}

static json_Status parseArray(ds_Vector_token* tokenList, size_t start, json_Value* output, size_t* offset) {
  output->type = json_Array;
  json_Status status = json_status_OK;
  *offset = 0;

  ds_Vector_value* array = (ds_Vector_value*) malloc(sizeof(ds_Vector_value));
  if (array == NULL) {
    status = json_status_AllocError;
    return status;
  }
  ds_vec_initVector_value(array, 0);
  
  output->data.arr = array;
  
  size_t i = start;
  while (tokenList->buf[i].type != RightSquareBr && i < tokenList->len) {
    Token* token = tokenList->buf + i;
    if (IS_VALUE(token)) {
      json_Value value;
      status = parseValue(tokenList, i, &value, offset);

      if (status != json_status_OK) {
        *offset = 0;
        return status;
      }
      i += *offset;

      ds_vec_appendElement_value(array, value);
    }
    // else if (token->type == RightSquareBr) {
    //   i++;
    //   break;
    // }
    else if (token->type != Comma) {
      status = json_status_InvalidInput;
      *offset = 0;
      return status;
    }

    i++;
  }

  *offset = i - start + 1;
  return status;
}

static json_Status parseObject(ds_Vector_token* tokenList, size_t start, json_Value* output, size_t* offset) {
  json_Status status;

  output->type = json_Object;
  status = json_status_OK;

  ds_um_Map_value* map = (ds_um_Map_value*) malloc(sizeof(ds_um_Map_value));
  if (map == NULL) {
    status = json_status_AllocError;
    *offset = 0;
    return status;
  }
  ds_um_initMap_value(map);

  output->data.obj = map;

  size_t i = start;
  while (tokenList->buf[i].type != RightCurlyBr && i < tokenList->len) {
    Token* token = tokenList->buf + i;
    if (token->type == String) {
      json_Value key;
      json_Status keyStatus = parseString(token, &key);
      if (keyStatus != json_status_OK) {
        status = keyStatus;
        *offset = 0;
        return keyStatus;
      }

      i++;
      token = tokenList->buf + i;

      if (token->type != Colon) {
        status = json_status_InvalidInput;
        *offset = 0;
        return status;
      }

      i++;
      token = tokenList->buf + i;

      json_Value value;
      json_Status valueStatus = parseValue(tokenList, i, &value, offset);
      if (valueStatus != json_status_OK) {
        status = valueStatus;
        *offset = 0;
        return valueStatus;
      }
      i += *offset;

      ds_um_insertStrk_value(map, key.data.string, value);

      json_destroy(&key);
    }
    else if (token->type == RightCurlyBr) {
      i++;
      break;
    }
    else if (token->type != Comma) {
      status = json_status_InvalidInput;
      *offset = 0;
      return status;
    }

    i++;
  }

  *offset = i - start + 1;
  return status;
}

json_Status json_parser(const char* input, json_Value* output) {
  json_Status ret = json_status_OK;

  ds_Vector_token tokenList;
  ds_vec_initVector_token(&tokenList, 0);

  lexer(input, &tokenList);
  if (tokenList.len == 0) {
    ret = json_status_InvalidInput;
    return ret;
  }

  size_t offset = 0;
  ret = parseValue(&tokenList, 0, output, &offset);

  ds_vec_destroyVector_token(&tokenList);

  return ret;
}
