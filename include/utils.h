#include "status.h"
#include "../lib/data_structures/status/status.h"

#define RETURN_DS_ERROR(func, var_name) \
var_name = func;\
if (var_name != DS_OK) {\
  return dsStatusToJsonStatus(var_name);\
}

#define RETURN_JSON_ERROR(func, var_name) \
var_name = func;\
if (var_name != json_status_OK) {\
  return var_name;\
}

json_Status dsStatusToJsonStatus(ds_status status);

int hexChrToInt(char chr);
