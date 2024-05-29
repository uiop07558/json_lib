# json_lib

Simple and dirty JSON processing library written in C. Made as hobby project, is not intended for use in production.

## Usage

#### Header

To use library you need to include header json.h

```c
#include "/root-of-library/include/json.h"
```

#### Parsing

There two versions of parsers: json\_parser for null-terminated string and json\_parserNoNul for general string buffers. Both return status code and put their output in passed pointer. Input string should be in UTF-8.

```c
json_Status json_parser(const char* input, json_Value* output);
```

* Return value - status code
* `input` - null-terminated input string
* `output` - pointer to struct of type `json_Value` in which parser will put root value node (e.g. array or object) of json document

```c
json_Status json_parserNoNul(const char* input, size_t len, json_Value* output);
```

* Return value - status code
* `input` - input string
* `len` - length of input string
* `output` - pointer to struct of type `json_Value` in which parser will put root node (e.g. array or object) of value tree of json document

###### Example

```c
char* jsonStringNulTerm = "{\"json\": []}"; // null-terminated string

json_Value root;
json_Status status = json_parser(jsonStringNulTerm, &root);
```

```c
char jsonStringNoNulTerm[2] = {'{', '}'}; // not null-terminated

json_Value root;
json_Status status = json_parserNoNul(jsonStringNoNulTerm, 2, &root);
```

#### Stringifiying

Converting JSON value tree into string is done by json\_stringifier. It returns status code and puts its output into dynamically allocated `char` buffer. Its address and size are being put into argument pointers. Output string is in UTF-8. String is null-terminated. If some JSON strings in tree contain NUL characters, they will be added to output as any other character.

```c
json_Status json_stringify(json_Value* value, char** output, size_t* len);
```

* Return value - status code
* `value` - input JSON value
* `output` - pointer to `char` pointer in which function will put address of output string buffer
* `len` - pointer to variable in which function will put length of output string

###### Example

```c
char* output = NULL;
size_t outputLen = 0;

json_Status status = json_stringifier(&root, &output, &outputLen);
```

#### Status codes

Represented by enum of type `json_Status`

* `json_status_OK` - returned if function executed normally
* `json_status_InvalidInput` - returned if function found that input is invalid _(NOTE: validation is not guaranteed, parser may accept some invalid input strings)_
* `json_status_AllocError` - returned if there is error from memory allocator (e.g. out of memory situation)
* `json_status_ElemNotExist` - returned if asked element does not exist _(NOTE: currently no function uses it)_

#### Value type

Represented by struct of type `json_Value` It has two fields:

* `type` - type of JSON value. Represented by enum `json_ValueType`, its values are `json_Object`, `json_Array`, `json_String`, `json_Number`, `json_Boolean`, `json_Null`, they associate with built-in JSON types*
* `value` - value of given type. Represented by union `json_Data`, its fields are:
    * `number` - floating point number of type `double`, valid if `type` is `json_Number`
    * `boolean` - boolean value of type bool (from standard library), valid if `type` is `json_Boolean`
    * `string` - pointer to null-terminated string (`char` array), valid if `type` is `json_String`
    * `arr` - pointer to struct of type `ds_Vector_value` which is dynamic vector (from `data_structures` library) containing other JSON values, valid if `type` is `json_Array`
    * `obj` - pointer to struct of type `ds_um_Map_value` which is hash map (from `data_structures` library) containing other JSON values, valid if `type` is `json_Object`
