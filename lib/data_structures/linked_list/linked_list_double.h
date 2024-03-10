#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../status/status.h"

#define DS_LL_START_CAPACITY 1024
#define DS_LL_CAPACITY_EXPANSION_FACTOR 1.4
#define DS_LL_DELCNT_THRESHOLD 0.4

#define ds_lld_initType(T, name) \
typedef struct ds_LinkedListDouble_##name {\
  unsigned int isCircular: 1;\
\
  size_t cap;\
  size_t bufpnt;\
  size_t delcnt;\
  struct ds_LinkedListNodeDouble_##name* buf;\
\
  size_t len;\
  struct ds_LinkedListNodeDouble_##name* start;\
  struct ds_LinkedListNodeDouble_##name* end;\
} ds_LinkedListDouble_##name;\
\
typedef struct ds_LinkedListNodeDouble_##name {\
  T value;\
  struct ds_LinkedListNodeDouble_##name* next;\
  struct ds_LinkedListNodeDouble_##name* prev;\
} ds_LinkedListNodeDouble_##name;\
\
ds_status ds_lld_initLinkedList_##name(ds_LinkedListDouble_##name* linkedList, bool isCircular) {\
  linkedList->isCircular = isCircular;\
  linkedList->cap = DS_LL_START_CAPACITY;\
  \
  ds_LinkedListNodeDouble_##name* buffer = (ds_LinkedListNodeDouble_##name*) malloc(linkedList->cap * sizeof(ds_LinkedListNodeDouble_##name));\
  if (buffer == NULL) {\
    return DS_ALLOC_ERROR;\
  }\
\
  linkedList->buf = buffer;\
  linkedList->bufpnt = 0;\
  linkedList->len = 0;\
  linkedList->start = NULL;\
  linkedList->end = NULL;\
\
  return DS_OK;\
}\
\
ds_status ds_lld__copyBuffer_##name(ds_LinkedListDouble_##name* linkedList, size_t size) {\
  ds_LinkedListNodeDouble_##name* newBuffer = (ds_LinkedListNodeDouble_##name*) malloc(size);\
  if (newBuffer == NULL) {\
    return DS_ALLOC_ERROR;\
  }\
\
  ds_LinkedListNodeDouble_##name* curNode = linkedList->start;\
  newBuffer[0] = *curNode;\
  for (size_t i = 1; i < linkedList->len; i++) {\
    curNode = curNode->next;\
\
    newBuffer[i] = *curNode;\
    newBuffer[i].prev = &newBuffer[i-1];\
    newBuffer[i-1].next = &newBuffer[i];\
  }\
\
  size_t newBufpnt = linkedList->len;\
  if (linkedList->isCircular) {\
    newBuffer[0].prev = &newBuffer[newBufpnt - 1];\
    newBuffer[newBufpnt - 1].next = &newBuffer[0];\
  }\
  else {\
    newBuffer[0].prev = NULL;\
    newBuffer[newBufpnt - 1].next = NULL;\
  }\
\
  free(linkedList->buf);\
\
  linkedList->buf = newBuffer;\
  linkedList->bufpnt = newBufpnt;\
  linkedList->start = &newBuffer[0];\
  linkedList->end = &newBuffer[newBufpnt - 1];\
\
  return DS_OK;\
}\
\
ds_status ds_lld__expandCapacity_##name(ds_LinkedListDouble_##name* linkedList) {\
  size_t newCap = (size_t) (linkedList->cap * DS_LL_CAPACITY_EXPANSION_FACTOR);\
  return ds_lld__copyBuffer_##name(linkedList, newCap * sizeof(ds_LinkedListNodeDouble_##name));\
}\
\
ds_status ds_lld__addNodeToBuffer_##name(ds_LinkedListDouble_##name* linkedList, ds_LinkedListNodeDouble_##name node) {\
  if (linkedList->bufpnt >= linkedList->cap) {\
    ds_status status = ds_lld__expandCapacity_##name(linkedList);\
    if (status != DS_OK) {\
      return status;\
    }\
  }\
  \
  linkedList->buf[linkedList->bufpnt] = node;\
  linkedList->bufpnt++;\
\
  return DS_OK;\
}\
\
ds_status ds_lld_cleanBuffer_##name(ds_LinkedListDouble_##name* linkedList) {\
  return ds_lld__copyBuffer_##name(linkedList, linkedList->cap * sizeof(ds_LinkedListNodeDouble_##name));\
}\
\
ds_status ds_lld_insertFirstNode_##name(ds_LinkedListDouble_##name* linkedList, T value) {\
  ds_LinkedListNodeDouble_##name newNode;\
  newNode.value = value;\
\
  ds_status allocStatus = ds_lld__addNodeToBuffer_##name(linkedList, newNode);\
  if (allocStatus == DS_ALLOC_ERROR) {\
    return DS_ALLOC_ERROR;\
  }\
  ds_LinkedListNodeDouble_##name* newNodeptr = &linkedList->buf[linkedList->bufpnt - 1];\
\
  if (linkedList->isCircular) {\
    newNodeptr->next = newNodeptr;\
    newNodeptr->prev = newNodeptr;\
  }\
  else {\
    newNodeptr->next = NULL;\
    newNodeptr->prev = NULL;\
  }\
\
  linkedList->start = newNodeptr;\
  linkedList->end = newNodeptr;\
\
  linkedList->len++;\
\
  return DS_OK;\
}\
\
ds_status ds_lld_insertNode_##name(ds_LinkedListDouble_##name* linkedList, ds_LinkedListNodeDouble_##name* nearNode, bool isAfter, T value) {\
  if (linkedList->len == 0) {\
    return ds_lld_insertFirstNode_##name(linkedList, value);\
  }\
\
  ds_LinkedListNodeDouble_##name newNode;\
  newNode.value = value;\
  if (isAfter) {\
    newNode.next = nearNode->next;\
    newNode.prev = nearNode;\
  }\
  else {\
    newNode.prev = nearNode->prev;\
    newNode.next = nearNode;\
  }\
\
  ds_status allocStatus = ds_lld__addNodeToBuffer_##name(linkedList, newNode);\
  if (allocStatus == DS_ALLOC_ERROR) {\
    return DS_ALLOC_ERROR;\
  }\
  ds_LinkedListNodeDouble_##name* newNodeptr = &linkedList->buf[linkedList->bufpnt - 1];\
\
  bool isEnd = nearNode == linkedList->end && isAfter;\
  bool isStart = nearNode == linkedList->start && !isAfter;\
\
  if (isEnd) {\
    linkedList->end = newNodeptr;\
    nearNode->next = newNodeptr;\
\
    if (linkedList->isCircular) {\
      linkedList->start->prev = newNodeptr;\
    }\
  }\
  else if (isStart) {\
    linkedList->start = newNodeptr;\
    nearNode->prev = newNodeptr;\
    if (linkedList->isCircular) {\
      linkedList->end->next = newNodeptr;\
    }\
  }\
  else {\
    if (isAfter) {\
      nearNode->next->prev = newNodeptr;\
      nearNode->next = newNodeptr;\
    }\
    else {\
      nearNode->prev->next = newNodeptr;\
      nearNode->prev = newNodeptr;\
    }\
  }\
\
  linkedList->len++;\
\
  return DS_OK;\
}\
\
ds_status ds_lld_deleteNode_##name(ds_LinkedListDouble_##name* linkedList, ds_LinkedListNodeDouble_##name* curNode) {\
  if (curNode->next != NULL) {\
    curNode->next->prev = curNode->prev;\
  }\
  if (curNode->prev != NULL) {\
    curNode->prev->next = curNode->next;\
  }\
\
  if (curNode == linkedList->start) {\
    linkedList->start = curNode->next;\
  }\
  if (curNode == linkedList->end) {\
    linkedList->end = curNode->prev;\
  }\
\
  linkedList->len--;\
  linkedList->delcnt++;\
\
  if (linkedList->delcnt / linkedList->cap >= DS_LL_DELCNT_THRESHOLD) {\
    return ds_lld_cleanBuffer_##name(linkedList);\
  }\
  \
  return DS_OK;\
}\
\
ds_status ds_lld_destroyLinkedList_##name(ds_LinkedListDouble_##name* linkedList) {\
  free(linkedList->buf);\
  return DS_OK;\
}