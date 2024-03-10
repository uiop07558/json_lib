#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "../status/status.h"

#define DS_VEC_INIT_TYPE_HEADER(T, name)\
typedef struct ds_Vector_##name {\
  size_t len;\
  size_t cap;\
  T* buf;\
} ds_Vector_##name;\
\
ds_status ds_vec_initVector_##name(ds_Vector_##name* vec, size_t elemNumber);\
\
static ds_status ds_vec__expandCapacity_##name(ds_Vector_##name* vec);\
\
ds_status ds_vec_appendElement_##name(ds_Vector_##name* vec, T elem);\
\
ds_status ds_vec_deleteElement_##name(ds_Vector_##name* vec, size_t index);\
\
ds_status ds_vec_insertElement_##name(ds_Vector_##name* vec, size_t index, T value);\
\
ds_status ds_vec_destroyVector_##name(ds_Vector_##name* vec);