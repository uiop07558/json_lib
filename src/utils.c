#include "../include/utils.h"

json_Status dsStatusToJsonStatus(ds_status status) {
  switch (status) {
  case DS_ALLOC_ERROR:
    return json_status_AllocError;
    break;
  case DS_ELEM_NOT_EXIST:
    return json_status_ElemNotExist;
    break;
  case DS_OK:
    return json_status_OK;
    break;
  default:
    return json_status_OK;
    break;
  }
}

int hexChrToInt(char chr) {
  if (chr >= '0' && chr <= '9') {
    return chr - '0';
  }
  
  if (chr >= 'A' && chr <= 'F') {
    return 10 + chr - 'A';
  }

  if (chr >= 'a' && chr <= 'f') {
    return 10 + chr - 'a';
  }

  return -1;
}