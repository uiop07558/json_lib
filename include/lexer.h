#pragma once

#include "token_vector_def.h"
#include "token.h"

void lexer(const char* input, size_t len, ds_Vector_token* tokenList);