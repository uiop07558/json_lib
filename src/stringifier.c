#include "../include/stringifier.h"

#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <float.h>

#include "../include/char_vector_def.h"
#include "../include/value_vector_def.h"
#include "../include/value_map_def.h"
#include "../include/status.h"
#include "../include/value.h"
#include "../include/utils.h"

json_Status stringifyValue(json_Value* value, ds_Vector_char* output);
json_Status stringifyArray(json_Value* value, ds_Vector_char* output);
json_Status stringifyObject(json_Value* value, ds_Vector_char* output);

json_Status stringifyStringInternal(char* str, size_t len, ds_Vector_char* output) {
  ds_vec_appendElement_char(output, '"');

  for (size_t i = 0; i < len; i++) {
    char curChr = str[i];
    if (curChr == '\\') {
      ds_vec_concatArray_char(output, "\\\\", 2);
    }
    else if (curChr == '"') {
      ds_vec_concatArray_char(output, "\\\"", 2);
    }
    else if (curChr == '\b') {
      ds_vec_concatArray_char(output, "\\b", 2);
    }
    else if (curChr == '\f') {
      ds_vec_concatArray_char(output, "\\f", 2);
    }
    else if (curChr == '\n') {
      ds_vec_concatArray_char(output, "\\n", 2);
    }
    else if (curChr == '\r') {
      ds_vec_concatArray_char(output, "\\r", 2);
    }
    else if (curChr == '\t') {
      ds_vec_concatArray_char(output, "\\t", 2);
    }
    else {
      ds_vec_appendElement_char(output, curChr);
    }
  }
  ds_vec_appendElement_char(output, '"');

  return json_status_OK;
}

json_Status stringifyString(json_Value* value, ds_Vector_char* output) {
  size_t len = strlen(value->data.string);
  return stringifyStringInternal(value->data.string, len, output);
}

json_Status stringifyNumber(json_Value* value, ds_Vector_char* output) {
  double number = value->data.number;
  if (isnan(number) || isinf(number)) {
    ds_vec_concatArray_char(output, "null", 4);
  }
  else {
    char numString[256] = {'\0'};
    size_t len = snprintf(numString, 256, "%g", number);
    ds_vec_concatArray_char(output, numString, len);
  }

  return json_status_OK;
}

json_Status stringifyValue(json_Value* value, ds_Vector_char* output) {
  if (value->type == json_String) {
    return stringifyString(value, output);
  }
  else if (value->type == json_Number) {
    return stringifyNumber(value, output);
  }
  else if (value->type == json_Boolean) {
    if (value->data.boolean == true) {
      ds_vec_concatArray_char(output, "true", 4);
    }
    else {
      ds_vec_concatArray_char(output, "false", 5);
    }
  }
  else if (value->type == json_Null) {
    ds_vec_concatArray_char(output, "null", 4);
  }
  else if (value->type == json_Object) {
    return stringifyObject(value, output);
  }
  else if (value->type == json_Array) {
    return stringifyArray(value, output);
  }

  return json_status_OK;
}

json_Status stringifyArray(json_Value* value, ds_Vector_char* output) {
  json_Status errStatus;
  ds_vec_appendElement_char(output, '[');
  for (size_t i = 0; i < value->data.arr->len; i++) {
    RETURN_JSON_ERROR(stringifyValue(value->data.arr->buf + i, output), errStatus)
    ds_vec_appendElement_char(output, ',');
  }
  RETURN_JSON_ERROR(stringifyValue(value->data.arr->buf + value->data.arr->len - 1, output), errStatus)
  ds_vec_appendElement_char(output, ']');

  return json_status_OK;
}

json_Status stringifyObject(json_Value* value, ds_Vector_char* output) {
  json_Status errStatus;

  ds_vec_appendElement_char(output, '{');
  
  ds_um_Iterator_value iter;
  ds_um_initIterator_value(value->data.obj, &iter);

  while (ds_um_iterate_value(&iter) != DS_ELEM_NOT_EXIST) {
    RETURN_JSON_ERROR(stringifyStringInternal(iter.curNode->key, iter.curNode->keySize - 1, output), errStatus)
    ds_vec_appendElement_char(output, ':');
    RETURN_JSON_ERROR(stringifyValue(&iter.curNode->value, output), errStatus)
    ds_vec_appendElement_char(output, ',');
  }
  if (output->buf[output->len - 1] == ',') {
    output->buf[output->len - 1] = '}';
  }
  else {
    ds_vec_appendElement_char(output, '}');
  }

  return json_status_OK;
}

json_Status json_stringify(json_Value* value, char** output, size_t* len) {
  ds_Vector_char chars;
  ds_vec_initVector_char(&chars, 0);

  json_Status status = stringifyValue(value, &chars);
  ds_vec_appendElement_char(&chars, '\0');

  *output = (char*) calloc(chars.len, sizeof(char));
  memcpy(*output, chars.buf, chars.len);

  *len = chars.len;

  ds_vec_destroyVector_char(&chars);

  return status;
}