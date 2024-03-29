#include "util.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "analysis/nodes.h"
#include "analysis/C.tab.h"

extern FILE *yyin;
extern NODE *ans;
extern NODE *yyparse();
extern int V,v;
extern FRAME *gbl_frame;

/* General Print Utils */
void print_banner(char *contents) {
  printf("%s\n%s\n%s\n", LINE, contents, LINE);
}

/* Prints string representation of input node type */
char *named(int t) {
  static char b[100];
  if (isgraph(t) || t==' ') {
    // Write the symbol to the string at identifier b
    sprintf(b, "%c", t);
    return b;
  }
  switch (t) {
   default: return "???";
   case IDENTIFIER:
    return "id";
   case CONSTANT:
    return "constant";
   case STRING_LITERAL:
    return "string";
   case LE_OP:
    return "<=";
   case GE_OP:
    return ">=";
   case EQ_OP:
    return "==";
   case NE_OP:
    return "!=";
   case EXTERN:
    return "extern";
   case AUTO:
    return "auto";
   case INT:
    return "int";
   case VOID:
    return "void";
   case APPLY:
    return "apply";
   case LEAF:
    return "leaf";
   case IF:
    return "if";
   case ELSE:
    return "else";
   case WHILE:
    return "while";
   case CONTINUE:
    return "continue";
   case BREAK:
    return "break";
   case RETURN:
    return "return";
   /* TAC */
   case LOAD:
    return "load";
   case PUSH:
    return "push";
   case GOTO:
    return "goto";
   case POP:
    return "pop";
   case START:
    return "start";
   case END:
    return "end";
  }
}

/* Prints string representation of node's token's type */
void print_leaf(NODE *tree, int level) {
  TOKEN *t = (TOKEN *)tree;
  int i;
  for (i=0; i<level; i++) putchar(' ');
  if (t->type == CONSTANT) {
    printf("%d (%s)\n", t->value, type_to_str(t->type));
  } else {
    printf("%s (%s)\n", t->lexeme, type_to_str(t->type));
  }
}

void print_branch(NODE *tree) {
    printf("%s\n", named(tree->type));
}

void print_tree0(NODE *tree, int level) {
  int i;
  if (tree==NULL) return;
  if (tree->type==LEAF) {
    print_leaf(tree->left, level);
  }
  else {
    for(i=0; i<level; i++) putchar(' ');
    print_branch(tree);
    print_tree0(tree->left, level+2);
    print_tree0(tree->right, level+2);
  }
}

void print_tree(NODE *tree) {
  print_tree0(tree, 0);
}

// Convenience stringeriser
char *int_to_str(int a) {
  static char str[10];
  sprintf(str, "%d", a);
  return str;
}
/* Returns the root of the true subtree, given an IF node */
NODE *get_true_root(NODE *n) {
  return n->right->type == ELSE ? n->right->left  : n->right;
}

/* Returns the root of the false subtree, given an IF node */
NODE *get_false_root(NODE *n) {
  return n->right->type == ELSE ? n->right->right : NULL;
}

/* Input File Utils */
void set_input_file(char *filename) {
  printf("Setting input file to %s\n", filename);
  yyin = fopen(filename, "r");
  if (yyin == NULL && filename != NULL) perror("Input file error!");
}

void close_input_file() {
  if (yyin) fclose(yyin);
}

void parse(char *filename) {
  printf("Parsing...\n");
  if (filename != NULL) {
    set_input_file(filename);
  }
  if (v) print_banner(filename);
  init_environment();
  if (v) printf("yyparsing\n");
  yyparse();
  if (v) print_tree(ans);
  if (V) print_environment(gbl_frame);
  close_input_file();
}
