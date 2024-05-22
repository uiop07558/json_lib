#pragma once

#include "../include/status.h"
#include "../include/value.h"

json_Status json_stringify(json_Value* value, char* output, size_t* len);