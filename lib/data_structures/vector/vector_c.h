#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../status/status.h"

#define DS_VEC_CAPACITY_EXPANSION_FACTOR 2

#define DS_VEC_INIT_TYPE_CODE(T, name) \
ds_status ds_vec_initVector_##name(ds_Vector_##name* vec, size_t elemNumber) {\
  vec->len = elemNumber;\
  vec->cap = elemNumber;\
  if (elemNumber == 0) {\
    vec->cap = 1;\
  }\
\
  vec->buf = (T*)malloc(vec->cap * sizeof(T));\
\
  return DS_OK;\
}\
\
static ds_status ds_vec__expandCapacity_##name(ds_Vector_##name* vec, size_t addlen) {\
  size_t newCap = vec->cap * DS_VEC_CAPACITY_EXPANSION_FACTOR;\
  if (newCap <= vec->len + addlen) {\
    newCap += addlen;\
  }\
\
  T* newBuffer = realloc(vec->buf, (size_t)(newCap * sizeof(T)));\
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
ds_status ds_vec_appendElement_##name(ds_Vector_##name* vec, T elem) {\
  if (vec->len >= vec->cap) {\
    ds_status status = ds_vec__expandCapacity_##name(vec, 1);\
\
    if (status == DS_ALLOC_ERROR) {\
      return DS_ALLOC_ERROR;\
    }\
  }\
\
  vec->len = vec->len + 1;\
  vec->buf[(vec->len)-1] = elem;\
\
  return DS_OK;\
}\
\
ds_status ds_vec_concatArray_##name(ds_Vector_##name* vec, T* array, size_t arrlen) {\
  if (vec->len + arrlen > vec->cap) {\
    ds_status status = ds_vec__expandCapacity_##name(vec, arrlen);\
\
    if (status == DS_ALLOC_ERROR) {\
      return DS_ALLOC_ERROR;\
    }\
  }\
\
  memcpy(vec->buf + vec->len, array, arrlen * sizeof(T));\
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
