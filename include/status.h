#pragma once

typedef enum json_Status {
  json_status_OK,
  json_status_InvalidInput,
  json_status_AllocError,
  json_status_ElemNotExist
} json_Status;