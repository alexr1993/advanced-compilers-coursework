#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "environment.h"
#include "C.tab.h"
#include "token.h"
extern int V;

TOKEN *new_token(int type)
{
    TOKEN *ans = (TOKEN*)malloc(sizeof(TOKEN));
    ans->type = type;
    ans->newly_created = true;
    return ans;
}

TOKEN *make_string(char *s)
{
    TOKEN *ans = new_token(STRING_LITERAL);
    int len = strlen(s);
    ans->lexeme = (char*)calloc(1, len-1);
    strncpy(ans->lexeme, s+1, len-2);
    return ans;
}

TOKEN *make_int(char *s)
{
    int n = *s!='\'' ? atoi(s) : *(s+1);
    TOKEN *ans = new_token(CONSTANT);
    ans->value = n;
    return ans;
}

TOKEN *make_identifier(char *s)
{
    TOKEN *ans = new_token(IDENTIFIER);
    ans->lexeme = (char*)malloc(1+strlen(s));
    strcpy(ans->lexeme, s);
    return ans;
}

/* The type of the token is not the type of the value which identifiers
 * point to */
char *type_to_str(int type) {
  switch(type) {
   case STRING_LITERAL:
    return "STRING_LITERAL";
   case CONSTANT:
    return "CONSTANT";
   case IDENTIFIER:
    return "IDENTIFIER";
   default:
    return "TYPE_NOT_RECOGNISED";
  }
}

void print_token(TOKEN *t) {
  switch(t->type) {
   case IDENTIFIER:
    printf("TOKEN \"%s\" (lexeme), type: IDENTIFIER, data_type: %s, param: %s\n",
            t->lexeme,
            data_type_to_str(t->data_type),
            t->declaration_type == PARAMETER ? "yes" : "no");
    break;
   case CONSTANT:
    printf("TOKEN \"%d\" (value), type: CONSTANT\n", t->value);
    break;
  }
}

/* The token stack is useful for populating frames during parsing */

void init_token_stack() {
    ts = malloc(sizeof(struct token_stack));
    ts->size = 0;
    ts->top = NULL;
}

/* Push down the token stack */
void push(TOKEN *t) {
  if (ts->size == 0) {
    ts->top = t;
  } else {
    t->next = ts->top;
    ts->top = t;
  }
  ts->size++;
}

/* Return the top of the token stack */
TOKEN *pop() {
  if (ts->size == 0) return NULL;
  TOKEN *rtn = ts->top;
  ts->top = ts->top->next;
  rtn->next = NULL;
  ts->size--;
  return rtn;
}

void print_token_stack() {
  TOKEN *t = ts->top;
  printf("TOKEN STACK CONTENTS\n");
  printf("========================================\n");
  while (t != NULL) {
    print_token(t);
    t = t->next;
  }
  printf("========================================\n");
}

/* Sets all of the tokens in the stack as parameter identifiers */
void mark_params() {
    TOKEN *t = ts->top;
    while (t != NULL) {
        t->declaration_type = PARAMETER;
        t = t->next;
    }
}

