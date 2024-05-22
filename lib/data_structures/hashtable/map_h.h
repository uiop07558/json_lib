#pragma once

#include "hash.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "../status/status.h"

#define DS_UM_INIT_TYPE_HEADER(T, name)\
typedef struct ds_um_Node_##name {\
  unsigned int _isFilled: 1;\
  unsigned int _isDeleted: 1;\
  unsigned int _strMode: 1;\
\
  uint32_t keySize;\
  void* key;\
  T value;\
} ds_um_Node_##name;\
\
typedef struct ds_um_Map_##name {\
  ds_um_Node_##name* _buf;\
  uint32_t _cap;\
\
  uint32_t _delcnt;\
  uint32_t _fillcnt;\
\
  uint32_t len;\
} ds_um_Map_##name;\
\
typedef struct ds_um_Iterator_##name {\
  ds_um_Map_##name* _map;\
  uint32_t curIndex;\
\
  ds_um_Node_##name* curNode;\
} ds_um_Iterator_##name;\
\
typedef struct ds_um_ValueStatus_##name {\
  ds_status status;\
  T value;\
} ds_um_ValueStatus_##name;\
\
ds_status ds_um_initMap_##name(ds_um_Map_##name* map);\
\
ds_status ds_um_insert_##name(ds_um_Map_##name* map, void* key, uint32_t keySize, T value);\
\
ds_um_ValueStatus_##name ds_um_get_##name(ds_um_Map_##name* map, void* key, uint32_t keySize);\
\
ds_status ds_um_delete_##name(ds_um_Map_##name* map, void* key, uint32_t keySize);\
\
ds_status ds_um_destroyMap_##name(ds_um_Map_##name* map);\
\
ds_status ds_um_initIterator_##name(ds_um_Map_##name* map, ds_um_Iterator_##name* iter);\
\
ds_status ds_um_iterate_##name(ds_um_Iterator_##name* iter);\
\
ds_status ds_um_insertStrk_##name (ds_um_Map_##name* map, char* key, T value);\
\
ds_status ds_um_deleteStrk_##name (ds_um_Map_##name* map, char* key, T value);\
\
ds_um_ValueStatus_##name ds_um_getStrk_##name(ds_um_Map_##name* map, void* key);
