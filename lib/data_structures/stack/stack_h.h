#pragma once

#include <stdint.h>
#include "../status/status.h"

#define DS_S_INIT_TYPE_HEADER(T, name)\
typedef struct ds_Stack_##name ds_Stack_##name;\
\
ds_status ds_s_initStack(ds_Stack_##name* stack);\
\
ds_status ds_s_push(ds_Stack_##name* stack, T value);\
\
T ds_s_peek(ds_Stack_##name* stack);\
\
T ds_s_pop(ds_Stack_##name* stack);\
\
size_t ds_s_len(ds_Stack_##name* stack);\
\
ds_status ds_s_destroyStack(ds_Stack_##name* stack);
