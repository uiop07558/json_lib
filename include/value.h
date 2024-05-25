#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "status.h"
// #include "../include/value_vector_def.h"
// #include "../include/value_map_def.h"

typedef struct ds_Vector_value ds_Vector_value;
typedef struct ds_um_Map_value ds_um_Map_value;

typedef enum json_ValueType {
  json_Object = 1,
  json_Array,
  json_String,
  json_Number,
  json_Boolean,
  json_Null
} json_ValueType;

typedef union json_Data {
  double number;
  bool boolean;
  char* string;
  ds_Vector_value* arr;
  ds_um_Map_value* obj;
} json_Data;

typedef struct json_Value {
  json_Data data;
  json_ValueType type;
} json_Value;

typedef struct json_StatusValue {
  json_Value value;
  json_Status status;
} json_StatusValue;
