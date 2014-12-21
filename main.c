#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "analysis/nodes.h"
#include "analysis/token.h"
#include "analysis/C.tab.h"
#include "analysis/environment.h"

#include "synthesis/evaluate.h"
#include "synthesis/interpret.h"
#include "synthesis/tac.h"

#include "common/util.h"

extern int yyparse();
extern NODE *ans;
extern FRAME *gbl_frame;
extern int V, v;

char *filename;

void translate_to_TAC() {
  yyparse();
  NODE *tree = ans;
  print_tree(tree);
  //evaluate(tree, NULL, gbl_frame, TAC);
  return;
}

void translate_to_MIPS() {
  return;
}

void interpret_source(void) {
  if (v) printf("Starting parse + semantic analysis\n");
  parse(NULL);
  VALUE *output = call("main", gbl_frame);
  printf("Output: %d\n", output->state->integer);
}

int main(int argc, char *argv[]) {
  int c     = 0;
  int len;
  char *action  = NULL;
  V = 0; // Verbose
  v = 0;

  // Determine translation requested
  while ((c = getopt(argc, argv, "vVa:f:")) != -1) {
    switch (c) {
     case 'a':
      // copy action type to var
      len = strlen(optarg);
      action = (char *) malloc(len * sizeof(char));
      strncpy(action, optarg, len);

      printf("Action Selected: %s\n", action);
      break;

     case 'f':
      printf("Input source file: %s\n", optarg);
      set_input_file(optarg);
      break;

     case 'V':
      v = 1;
      V = 1;
      break;

     case 'v':
      v = 1;
      break;

     default:
      printf("Invalid argument.\n");
      abort();
    }
  }
  if (action == NULL) action = "interpret";

  /* Translate */
  if ( str_eq(action, "") ) {
    printf("No action selected\n");
    abort();
  }
  else if ( str_eq(action, "interpret") ) {
    interpret_source();
  }
  else if ( str_eq(action, "tac") ) {
    translate_to_TAC();
  }
  else if ( str_eq(action, "mips") ) {
    translate_to_MIPS(); // expects TAC input
  }
  else {
    printf("Action is invalid!\n");
    abort();
  }
  return 0;
}
