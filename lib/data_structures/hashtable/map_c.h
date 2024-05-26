#pragma once

#define __LOAD_FACTOR_THRESHOLD 0.7
#define __DEL_LOAD_FACTOR_THRESHOLD 0.6
#define __INIT_CAP 11
#define __HASH2PRIME_MIN_VALUE_DECREASE_FACTOR 0.4
#define __CAPACITY_EXPANSION_FACTOR 2

#define __max(a, b) ((a) > (b) ? (a) : (b))
#define __min(a, b) ((a) < (b) ? (a) : (b))

#define DS_UM_INIT_TYPE_CODE(T, name) \
static inline uint32_t findSmallestPrime(uint32_t start);\
\
static uint32_t findBucket(ds_um_Map_##name* map, void* key, uint32_t keySize);\
\
static ds_status rehash(ds_um_Map_##name* map, uint32_t cap);\
\
static inline ds_status resize(ds_um_Map_##name* map);\
\
static inline uint32_t getPos(ds_um_Map_##name* map, void* key, uint32_t keySize);\
\
static inline ds_status insertInternal(ds_um_Map_##name* map, void* key, uint32_t keySize, T value, bool strMode);\
\
static inline uint32_t findSmallestPrime(uint32_t start) {\
  for (uint32_t i = (start | 1); i < UINT32_MAX; i += 2) {\
    uint32_t lim = (uint32_t) sqrt(i);\
    bool isPrime = 1;\
    for (uint32_t div = 3; div <= lim; div++) {\
      if (i % div == 0) {\
        isPrime = 0;\
        break;\
      }\
    }\
\
    if (isPrime) {\
      return i;\
    }\
  }\
}\
\
static uint32_t findBucket(ds_um_Map_##name* map, void* key, uint32_t keySize) {\
  uint64_t initHash = ds_h_xxHash364((uint8_t*) key, keySize);\
  uint32_t hash1 = initHash % map->_cap;\
\
  uint32_t hash2Prime = findSmallestPrime((uint32_t) (map->_cap * __HASH2PRIME_MIN_VALUE_DECREASE_FACTOR));\
  uint32_t hash2 = hash2Prime - (initHash % hash2Prime);\
\
  for (uint32_t i = 0; i < map->_cap; i++) {\
    uint32_t index = (hash1 + i*hash2) % map->_cap;\
    if (map->_buf[index]._isDeleted || !map->_buf[index]._isFilled) {\
      return index;\
    }\
  }\
\
  return UINT32_MAX;\
}\
\
static ds_status rehash(ds_um_Map_##name* map, uint32_t cap) {\
  ds_um_Map_##name newMap;\
  newMap.len = 0;\
  newMap._delcnt = 0;\
  newMap._fillcnt = 0;\
  newMap._cap = cap;\
  newMap._buf = (ds_um_Node_##name*) calloc(newMap._cap, sizeof(ds_um_Node_##name));\
  if (map->_buf == NULL) {\
    return DS_ALLOC_ERROR;\
  }\
\
  for (uint32_t i = 0; i < map->_cap; i++) {\
    ds_um_Node_##name* curNodePtr = map->_buf + i;\
    if (curNodePtr->_isFilled && !curNodePtr->_isDeleted) {\
      ds_um_insert_##name(&newMap, curNodePtr->key, curNodePtr->keySize, curNodePtr->value);\
    }\
  }\
\
  free(map->_buf);\
  *map = newMap;\
\
  return DS_OK;\
}\
\
static inline ds_status resize(ds_um_Map_##name* map) {\
  uint32_t newCap = findSmallestPrime((uint32_t) map->_cap * __CAPACITY_EXPANSION_FACTOR);\
  return rehash(map, newCap);\
}\
\
static inline uint32_t getPos(ds_um_Map_##name* map, void* key, uint32_t keySize) {\
  uint64_t initHash = ds_h_xxHash364((uint8_t*) key, keySize);\
  uint32_t hash1 = initHash % map->_cap;\
\
  uint32_t hash2Prime = findSmallestPrime((uint32_t) (map->_cap * 0.4));\
  uint32_t hash2 = hash2Prime - (initHash % hash2Prime);\
\
  uint32_t index = UINT32_MAX;\
  for (uint32_t i = 0; i < map->_cap; i++) {\
    uint32_t curIndex = (hash1 + i*hash2) % map->_cap;\
    ds_um_Node_##name* curNodePtr = map->_buf + curIndex;\
    if (!curNodePtr->_isFilled) {\
      break;\
    }\
    else if (!curNodePtr->_isDeleted && keySize == curNodePtr->keySize && memcmp(key, curNodePtr->key, __min(keySize, curNodePtr->keySize)) == 0) {\
      index = curIndex;\
      break;\
    }\
  }\
\
  return index;\
}\
\
static inline ds_status insertInternal(ds_um_Map_##name* map, void* key, uint32_t keySize, T value, bool strMode) {\
  uint64_t initHash = ds_h_xxHash364((uint8_t*) key, keySize);\
  uint32_t hash1 = initHash % map->_cap;\
\
  uint32_t hash2Prime = findSmallestPrime((uint32_t) (map->_cap * __HASH2PRIME_MIN_VALUE_DECREASE_FACTOR));\
  uint32_t hash2 = hash2Prime - (initHash % hash2Prime);\
\
  uint32_t index = UINT32_MAX;\
  for (uint32_t i = 0; i < map->_cap; i++) {\
    uint32_t curIndex = (hash1 + i*hash2) % map->_cap;\
    if (map->_buf[curIndex]._isDeleted) {\
      map->_delcnt--;\
      index = curIndex;\
      break;\
    }\
    else if (!map->_buf[curIndex]._isFilled || memcmp(key, map->_buf[curIndex].key, __min(keySize, map->_buf[curIndex].keySize)) == 0) {\
      index = curIndex;\
      break;\
    }\
  }\
\
  if (index == UINT32_MAX) {\
    return DS_ELEM_NOT_EXIST;\
  }\
\
  map->_buf[index]._isDeleted = 0;\
  map->_buf[index]._isFilled = 1;\
  map->_buf[index]._strMode = strMode;\
  map->_buf[index].value = value;\
  map->_buf[index].key = key;\
  map->_buf[index].keySize = keySize;\
\
  map->len++;\
  map->_fillcnt++;\
\
  double loadFactor = (double) map->_fillcnt / (double) map->_cap;\
  double delLoadFactor = (double) map->_delcnt / (double) map->_cap;\
  if (loadFactor >= __LOAD_FACTOR_THRESHOLD) {\
    ds_status status = resize(map);\
    if (status != DS_OK) {\
      return status;\
    }\
  }\
  else if (delLoadFactor >= __DEL_LOAD_FACTOR_THRESHOLD) {\
    ds_status status = rehash(map, map->_cap);\
    if (status != DS_OK) {\
      return status;\
    }\
  }\
  \
  return DS_OK;\
}\
\
ds_status ds_um_initMap_##name(ds_um_Map_##name* map) {\
  map->len = 0;\
  map->_delcnt = 0;\
  map->_fillcnt = 0;\
\
  map->_cap = __INIT_CAP;\
  map->_buf = (ds_um_Node_##name*) calloc(map->_cap, sizeof(ds_um_Node_##name));\
  if (map->_buf == NULL) {\
    return DS_ALLOC_ERROR;\
  }\
  return DS_OK;\
}\
\
ds_status ds_um_insert_##name(ds_um_Map_##name* map, void* key, uint32_t keySize, T value) {\
  return insertInternal(map, key, keySize, value, 0);\
}\
\
ds_um_ValueStatus_##name ds_um_get_##name(ds_um_Map_##name* map, void* key, uint32_t keySize) {\
  uint32_t index = getPos(map, key, keySize);\
  ds_um_ValueStatus_##name ret;\
\
  if (index == UINT32_MAX) {\
    ret.status = DS_ELEM_NOT_EXIST;\
    return ret;\
  }\
\
  ret.status = DS_OK;\
  ret.value = map->_buf[index].value;\
  return ret;\
}\
\
ds_status ds_um_delete_##name(ds_um_Map_##name* map, void* key, uint32_t keySize) {\
  uint32_t index = getPos(map, key, keySize);\
\
  if (index == UINT32_MAX) {\
    return DS_ELEM_NOT_EXIST;\
  }\
\
  map->_delcnt++;\
  map->len--;\
\
  map->_buf[index]._isDeleted = 1;\
  return DS_OK;\
}\
\
ds_status ds_um_destroyMap_##name(ds_um_Map_##name* map) {\
  for (uint32_t i = 0; i < map->_cap; i++) {\
    ds_um_Node_##name* curNode = map->_buf + i;\
    if (curNode->_isFilled && !curNode->_isDeleted && curNode->_strMode) {\
      free(curNode->key);\
    }\
  }\
\
  free(map->_buf);\
  return DS_OK;\
}\
\
ds_status ds_um_initIterator_##name(ds_um_Map_##name* map, ds_um_Iterator_##name* iter) {\
  iter->curIndex = 0;\
  iter->_map = map;\
  iter->curNode = NULL;\
\
  return DS_OK;\
}\
\
ds_status ds_um_iterate_##name(ds_um_Iterator_##name* iter) {\
  if (iter->_map == NULL) return DS_ELEM_NOT_EXIST;\
  while (iter->curIndex < iter->_map->_cap) {\
    uint32_t i = iter->curIndex;\
    if (iter->_map->_buf[i]._isFilled && !iter->_map->_buf[i]._isDeleted) {\
      iter->curNode = iter->_map->_buf + i;\
      iter->curIndex++;\
      return DS_OK;\
    }\
    iter->curIndex++;\
  }\
\
  return DS_ELEM_NOT_EXIST;\
}\
\
ds_status ds_um_insertStrk_##name (ds_um_Map_##name* map, char* key, T value) {\
  uint32_t keySize = strlen(key) + 1;\
\
  char* storedKey = (char*) calloc(keySize, sizeof(char));\
  if (storedKey == NULL) {\
    return DS_ALLOC_ERROR;\
  }\
  strcpy(storedKey, key);\
\
  return insertInternal(map, storedKey, keySize, value, 1);\
}\
\
ds_status ds_um_deleteStrk_##name (ds_um_Map_##name* map, char* key, T value) {\
  uint32_t index = getPos(map, key, strlen(key) + 1);\
\
  if (index == UINT32_MAX) {\
    return DS_ELEM_NOT_EXIST;\
  }\
\
  map->_delcnt++;\
  map->len--;\
\
  ds_um_Node_##name* curNode = map->_buf + index;\
  curNode->_isDeleted = 1;\
  free(curNode->key);\
  return DS_OK;\
}\
\
ds_um_ValueStatus_##name ds_um_getStrk_##name (ds_um_Map_##name* map, void* key) {\
  return ds_um_get_##name(map, key, strlen(key)+1);\
}
