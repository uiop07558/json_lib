#include "../include/parser.h"
#include "../include/stringifier.h"
#include "../include/destructor.h"

#include "../include/status.h"
#include "../include/value.h"
#include "../include/value_map_def.h"
#include "../include/value_vector_def.h"

#include <stdio.h>
#include <stdlib.h>

void printValue(json_Value* value) {
  printf("Value type: %d\n", value->type);
  printf("Value data: %p\n\n", value->data.arr);

  if (value->type == json_Array && value->data.arr != NULL) {
    for (int64_t i = 0; i < value->data.arr->len; i++) {
      printValue(value->data.arr->buf + i);
    }
  }
  else if (value->type == json_Object && value->data.obj != NULL) {
    ds_um_Iterator_value iter;
    ds_um_initIterator_value(value->data.obj, &iter);

    while (ds_um_iterate_value(&iter) != DS_ELEM_NOT_EXIST) {
      printValue(&iter.curNode->value);
    }
  }
}

int main() {
  FILE* file = fopen("input.json", "r");
  if (file == NULL) {
    printf("File not found\n");
    return 1;
  }
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* str = calloc(size + 1, sizeof(char));
  if (str == NULL) {
    printf("Memory error\n");
    return 1;
  }
  fread(str, sizeof(char), size, file);

//==========================================================================

  json_Value json;
  if (json_parser(str, &json) != json_status_OK) {
    printf("Parsing error\n");
    return 1;
  }

  // printValue(&json);

  char* strJson = NULL;
  size_t strJsonLen = 0;

  if (json_stringify(&json, &strJson, &strJsonLen) != json_status_OK) {
    printf("Stringifying error\n");
    return 1;
  }

  json_destroy(&json);

//==========================================================================

  FILE* output = fopen("output.json", "w");
  if (output == NULL) {
    printf("Output file not found\n");
    return 1;
  }
  fprintf(output, "%s", strJson);

  fclose(file);
  fclose(output);
  free(str);
  free(strJson);
  return 0;
}