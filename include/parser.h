#pragma once

#include "../include/status.h"
#include "../include/value.h"

#include <stddef.h>

json_Status json_parser(const char* input, json_Value* output);

json_Status json_parserNoNul(const char* input, size_t len, json_Value* output);
