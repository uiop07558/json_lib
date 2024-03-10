#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../status/status.h"

#define DS_LL_START_CAPACITY 1024
#define DS_LL_CAPACITY_EXPANSION_FACTOR 1.4

#define ds_lls_initType(T, name) \
typedef struct ds_LinkedListNodeSingle_##name {\
  T value;\
  struct ds_LinkedListNodeSingle_##name* next;\
} ds_LinkedListNodeSingle_##name;\
\
typedef struct ds_LinkedListSingle_##name {\
  unsigned int isCircular: 1;\
\
  size_t cap;\
  size_t bufpnt;\
  size_t delcnt;\
  struct ds_LinkedListNodeSingle_##name* buf;\
\
  size_t len;\
  struct ds_LinkedListNodeSingle_##name* start;\
  struct ds_LinkedListNodeSingle_##name* end;\
} ds_LinkedListSingle_##name;\
\
\
ds_status ds_lls_initLinkedList_##name(ds_LinkedListSingle_##name* linkedList, bool isCircular) {\
  linkedList->isCircular = isCircular;\
  linkedList->cap = DS_LL_START_CAPACITY;\
  \
  ds_LinkedListNodeSingle_##name* buffer = (ds_LinkedListNodeSingle_##name*) malloc(linkedList->cap * sizeof(ds_LinkedListNodeSingle_##name));\
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
ds_status ds_lls__addNodeToBuffer_##name(ds_LinkedListSingle_##name* linkedList, ds_LinkedListNodeSingle_##name node) {\
  if (linkedList->bufpnt >= linkedList->cap) {\
    ds_status status = ds_lls__expandCapacity_##name(linkedList);\
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
ds_status ds_lls__expandCapacity_##name(ds_LinkedListSingle_##name* linkedList) {\
  size_t newCap = (size_t) (linkedList->cap * DS_LL_CAPACITY_EXPANSION_FACTOR);\
  return ds_lls__copyBuffer_##name(linkedList, newCap * sizeof(ds_LinkedListNodeSingle_##name));\
}\
\
ds_status ds_lls_cleanBuffer_##name(ds_LinkedListSingle_##name* linkedList) {\
  return ds_lls__copyBuffer_##name(linkedList, linkedList->cap * sizeof(ds_LinkedListNodeSingle_##name));\
}\
\
ds_status ds_lls__copyBuffer_##name(ds_LinkedListSingle_##name* linkedList, size_t size) {\
  ds_LinkedListNodeSingle_##name* newBuffer = (ds_LinkedListNodeSingle_##name*) malloc(size);\
  if (newBuffer == NULL) {\
    return DS_ALLOC_ERROR;\
  }\
\
  ds_LinkedListNodeSingle_##name* curNode = linkedList->start;\
  newBuffer[0] = *curNode;\
  for (size_t i = 1; i < linkedList->len; i++) {\
    curNode = curNode->next;\
\
    newBuffer[i] = *curNode;\
    newBuffer[i-1].next = &newBuffer[i];\
  }\
\
  size_t newBufpnt = linkedList->len;\
  if (linkedList->isCircular) {\
    newBuffer[newBufpnt - 1].next = &newBuffer[0];\
  }\
  else {\
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
ds_status ds_lls_insertFirstNode_##name(ds_LinkedListSingle_##name* linkedList, T value) {\
  ds_LinkedListNodeSingle_##name newNode;\
  newNode.value = value;\
\
  ds_status allocStatus = ds_lls__addNodeToBuffer_##name(linkedList, newNode);\
  if (allocStatus == DS_ALLOC_ERROR) {\
    return DS_ALLOC_ERROR;\
  }\
  ds_LinkedListNodeSingle_##name* newNodeptr = &linkedList->buf[linkedList->bufpnt - 1];\
\
  if (linkedList->isCircular) {\
    newNodeptr->next = newNodeptr;\
  }\
  else {\
    newNodeptr->next = NULL;\
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
ds_status ds_lls_insertNode_##name(ds_LinkedListSingle_##name* linkedList, ds_LinkedListNodeSingle_##name* prevNode, T value) {\
  if (linkedList->len == 0) {\
    return ds_lls_insertFirstNode_##name(linkedList, value);\
  }\
\
  ds_LinkedListNodeSingle_##name newNode;\
  newNode.value = value;\
  newNode.next = prevNode->next;\
\
  ds_status allocStatus = ds_lls__addNodeToBuffer_##name(linkedList, newNode);\
  if (allocStatus == DS_ALLOC_ERROR) {\
    return DS_ALLOC_ERROR;\
  }\
  ds_LinkedListNodeSingle_##name* newNodeptr = &linkedList->buf[linkedList->bufpnt - 1];\
\
  if (prevNode == linkedList->end) {\
    linkedList->end = newNodeptr;\
  }\
\
  prevNode->next = newNodeptr;\
\
  linkedList->len++;\
\
  return DS_OK;\
}\
\
ds_status ds_lls_insertNodeStart_##name(ds_LinkedListSingle_##name* linkedList, T value) {\
  if (linkedList->len == 0) {\
    return ds_lls_insertFirstNode_##name(linkedList, value);\
  }\
  \
  ds_LinkedListNodeSingle_##name newNode;\
  newNode.value = value;\
  newNode.next = linkedList->start;\
\
  ds_status allocStatus = ds_lls__addNodeToBuffer_##name(linkedList, newNode);\
  if (allocStatus == DS_ALLOC_ERROR) {\
    return DS_ALLOC_ERROR;\
  }\
  ds_LinkedListNodeSingle_##name* newNodeptr = &linkedList->buf[linkedList->bufpnt - 1];\
\
  if (linkedList->isCircular) {\
    linkedList->end->next = newNodeptr;\
    linkedList->start = newNodeptr;\
  }\
  else {\
    linkedList->start = newNodeptr;\
  }\
\
  linkedList->len++;\
\
  return DS_OK;\
}\
\
ds_status ds_lls_deleteNode_##name(ds_LinkedListSingle_##name* linkedList, ds_LinkedListNodeSingle_##name* prevNode) {\
  if (prevNode->next == linkedList->end) {\
    linkedList->end = prevNode;\
  }\
\
  prevNode->next = prevNode->next->next;\
\
  linkedList->len--;\
  linkedList->delcnt++;\
\
  if (linkedList->delcnt / linkedList->cap >= DS_LL_DELCNT_THRESHOLD) {\
    return ds_lls_cleanBuffer_##name(linkedList);\
  }\
  \
  return DS_OK;\
}\
\
ds_status ds_lls_deleteNodeStart_##name(ds_LinkedListSingle_##name* linkedList) {\
  linkedList->start = linkedList->start->next;\
\
  linkedList->len--;\
  linkedList->delcnt++;\
\
  if (linkedList->delcnt / linkedList->cap >= DS_LL_DELCNT_THRESHOLD) {\
    return ds_lls_cleanBuffer_##name(linkedList);\
  }\
\
  return DS_OK;\
}\
\
ds_status ds_lls_destroyLinkedList_##name(ds_LinkedListSingle_##name* linkedList) {\
  free(linkedList->buf);\
  return DS_OK;\
}