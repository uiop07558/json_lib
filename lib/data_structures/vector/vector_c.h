#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "../status/status.h"

#define DS_VEC_CAPACITY_EXPANSION_FACTOR 2

#define DS_VEC_INIT_TYPE_CODE(T, name) \
ds_status ds_vec_initVector_##name(ds_Vector_##name* vec, size_t elemNumber) {\
  vec->len = elemNumber;\
  vec->cap = elemNumber;\
\
  vec->buf = (int32_t*)malloc(elemNumber * sizeof(int32_t));\
\
  return DS_OK;\
}\
\
static ds_status ds_vec__expandCapacity_##name(ds_Vector_##name* vec) {\
  size_t newCap = vec->cap * DS_DEFAULT_CAPACITY_EXPANSION_FACTOR;\
\
  T* newBuffer = realloc(vec->buf, (size_t)(newCap * sizeof(int32_t)));\
\
  if (newBuffer == NULL) {\
    return DS_ALLOC_ERROR;\
  }\
\
  vec->cap = newCap;\
  vec->buf = newBuffer;\
\
  return DS_OK;\
}\
\
ds_status ds_vec_appendElement_##name(ds_Vector_##name* vec, T elemPtr) {\
  if (vec->len >= vec->cap) {\
    ds_status status = ds_vec__expandCapacity_##name(vec);\
\
    if (status == DS_ALLOC_ERROR) {\
      return DS_ALLOC_ERROR;\
    }\
  }\
\
  vec->len = vec->len + 1;\
  vec->buf[(vec->len)-1] = elemPtr;\
\
  return DS_OK;\
}\
\
ds_status ds_vec_deleteElement_##name(ds_Vector_##name* vec, size_t index) {\
  if (index >= vec->len || index < 0) {\
    return DS_ELEM_NOT_EXIST;\
  }\
\
  for (size_t i = index + 1; i < vec->len; i++) {\
    vec->buf[i-1] = vec->buf[i];\
  }\
\
  vec->buf[vec->len - 1] = NULL;\
  vec->len--;\
\
  return DS_OK;\
}\
\
ds_status ds_vec_insertElement_##name(ds_Vector_##name* vec, size_t index, T value) {\
  if (index >= vec->len || index < 0) {\
    return DS_ELEM_NOT_EXIST;\
  }\
\
  for (size_t i = (vec->len - 1); i >= index; i--) {\
    vec->buf[i+1] = vec->buf[i];\
  }\
\
  vec->buf[index] = value;\
  vec->len--;\
\
  return DS_OK;\
}\
\
ds_status ds_vec_destroyVector_##name(ds_Vector_##name* vec) {\
  free(vec->buf);\
  return DS_OK;\
}