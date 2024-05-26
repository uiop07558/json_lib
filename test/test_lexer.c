#include "../include/lexer.h"
#include <stdio.h>

void printToken(Token* token, char* input) {
  printf("Token type: %d\n", token->type);
  printf("Token length: %d\n", token->len);
  printf("Token string location: %d\n", token->str - input);
}

int main() {
  FILE* file = fopen("input.json", "r");
  if (file == NULL) {
    printf("File not found");
    return 1;
  }
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* str = calloc(size + 1, sizeof(char));
  if (str == NULL) {
    printf("Memory error");
    return 1;
  }
  fread(str, sizeof(char), size, file);

  ds_Vector_token tokens;
  ds_vec_initVector_token(&tokens, 0);
  
  lexer(str, size, &tokens);
  printf("size: %d\n", tokens.len);
  for (size_t i = 0; i < tokens.len; i++) {
    printToken(&tokens.buf[i], str);
    printf("\n");
    if (i > 100) {
      break;
    }
  }

  fclose(file);
  free(str);
  return 0;
}