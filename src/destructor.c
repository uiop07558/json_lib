#include "../include/destructor.h"


#include "../include/value_vector_def.h"
#include "../include/value_map_def.h"
#include "../include/status.h"
#include "../include/value.h"

json_Status json_destroy(json_Value* value) {
  if (value->type == json_String) {
    free(value->data.string);
  }
  else if (value->type == json_Array) {
    for (size_t i = 0; i < value->data.arr->len; i++) {
      json_destroy(value->data.arr->buf + i);
    }
    ds_vec_destroyVector_value(value->data.arr);
    free(value->data.arr);
  }
  else if (value->type == json_Object) {
    ds_um_Iterator_value iter;
    ds_um_initIterator_value(value->data.obj, &iter);
    while (ds_um_iterate_value(&iter) == DS_OK) {
      json_destroy(&iter.curNode->value);
    }
    ds_um_destroyMap_value(value->data.obj);
    free(value->data.obj);
  }

  return json_status_OK;
}