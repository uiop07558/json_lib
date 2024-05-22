#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../vector/vector_h.h"
#include "../vector/vector_c.h"
#include "../status/status.h"

#define DS_S_INIT_TYPE_CODE(T, name)\
DS_VEC_INIT_TYPE_HEADER(T, name)\
DS_VEC_INIT_TYPE_CODE(T, name)\
\
typedef struct ds_Stack_##name {\
  ds_Vector_##name vector;\
} ds_Stack_##name;\
\
ds_status ds_s_initStack(ds_Stack_##name* stack) {\
  return ds_vec_initVector_##name(&stack->vector, 0);\
}\
\
ds_status ds_s_push(ds_Stack_##name* stack, T value) {\
  return ds_vec_appendElement_##name(&stack->vector, value);\
}\
\
T ds_s_peek(ds_Stack_##name* stack) {\
  size_t index = stack->vector.len - 1;\
  return stack->vector.buf[index];\
}\
\
T ds_s_pop(ds_Stack_##name* stack) {\
  size_t index = stack->vector.len - 1;\
  T value = stack->vector.buf[index];\
  ds_vec_deleteElement_##name(&stack->vector, index);\
  return value;\
}\
\
size_t ds_s_len(ds_Stack_##name* stack) {\
  return stack->vector.len;\
}\
\
ds_status ds_s_destroyStack(ds_Stack_##name* stack) {\
  return ds_vec_destroyVector_##name(&stack->vector);\
}
