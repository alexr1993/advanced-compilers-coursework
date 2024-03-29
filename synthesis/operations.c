#include "operations.h"

#include "stdio.h"
#include "stdlib.h"
/*
 * Shim between evaluate.c and interpret.c | tac.c
 */

EVAL *evaluate_leaf(NODE *n, FRAME *f) {
  switch(e_type) {
   case INTERPRET:
    return new_eval((void *)interpret_leaf(n, f));
   case IR:
    return new_eval((void *)tac_leaf(n, f));
   default:
    perror("ERROR: Invalid eval type!\n");
    abort();
  }
}

EVAL *arithmetic(NODE *n, EVAL *l, EVAL *r, FRAME *f) {
  VALUE *val;
  switch(e_type) {
   case INTERPRET:
    val =  new_val(INT_TYPE, new_int_state(
      interpret_arithmetic(
        n->type, l->val->state->integer, r->val->state->integer)));
    return new_eval((void *)val);
   case IR:
    return new_eval((void *)tac_arith_and_logic(n, l->code, r->code));
   default:
    perror("Unknown EVAL_TYPE\n");
    exit(-1);
  }
}

EVAL *logic(NODE *n, EVAL *l, EVAL *r, FRAME *f) {
  VALUE *val;
  switch(e_type) {
   case INTERPRET:
    val = new_val(INT_TYPE, new_int_state(
      interpret_logic(n->type,
                      l->val->state->integer,
                      r->val->state->integer)));
    return new_eval((void *)val);
    break;

   case IR:
    return new_eval((void *)tac_arith_and_logic(n, l->code, r->code));
    break;

   default:
    perror("Unknown EVAL_TYPE\n");
    exit(-1);
  }
}

EVAL *control(NODE *n, EVAL *l, EVAL *r, FRAME *f) {
  switch(e_type) {
   case INTERPRET:
    return new_eval((void *)interpret_control(n,
                                              l ? l->val : NULL,
                                              r ? r->val : NULL, f));

   case IR:
    return new_eval((void *)tac_control(n,
                                        l == NULL ? NULL : l->code,
                                        r == NULL ? NULL : r->code, f));

   default:
    perror("Unknown EVAL_TYPE\n");
    exit(-1);
  }
}
