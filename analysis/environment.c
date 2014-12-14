#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "environment.h"

void print_env(ENV *env)
{
  ENV *current = env;
  if (!current) printf("    Frame contains no variables!");

  while (current)
  {
    printf("  Printing variable...\n");
    char * type;
    if (current->type == INT_TYPE) type = "INT";
    else type = "FN";

    printf("  Name: %s, Type: %s", current->name, type);

    if  (current->type == INT_TYPE)
    {
      printf(", Value: %d", current->state->value);
    }
    current = current->next;
    printf("\n");
  }
  printf("\n");
}

char *data_type_to_str(int type) {
  switch(type) {
   case INT_TYPE:
    return "INT_TYPE";
   case FN_TYPE:
    return "FN_TYPE";
   default:
    return "TYPE_NOT_RECOGNISED";
  }
}

void print_state(STATE *s, int type) {
  switch(type) {
   case INT_TYPE:
    printf("%d\n", s->value);
   case FN_TYPE:
    printf("function_state\n");
  }
}

void print_var(VARIABLE *var) {
  if (!var) {
    printf("VARIABLE not initialised\n");
    return;
  }
  printf("VARIABLE type: %s, ", data_type_to_str(var->type));
  print_state(var->state, var->type);
}

/*
 * Prints frame including all of it's children
 */
void print_frame(FRAME *frame)
{
  if (frame->parent == NULL) {
    printf("\nFRAME \"%s\"\n", frame->proc_id);
  } else {
    printf("\nFRAME \"%s\", child of \"%s\"\n",
           frame->proc_id,
           frame->parent->proc_id);
  }
  print_symbtable(frame->symbols, true);
}

 /* Adds env mapping to the environment */
void _add_env(ENV *env, FRAME *frame)
{
  // Insert env at the front of the frames variable list
  ENV *frame_var = frame->variable;
  env->next = frame_var;
  frame->variable = env;
  printf("Storing variable \"%s\"\n", env->name);
}

/*
 * Finds and returns the env mapping with the given name and type
 *
 * Checks given env first, then recursive looks at parent until var
 * is found
 *
 */
ENV *lookup_var(char *name, int type, FRAME *frame)
{
  printf("Looking up variable \"%s\"...", name);

  while (frame)
  {
    ENV *frame_var = frame->variable;

    /* Find the env mapping with the given name */
    while (frame_var)
    {
      // Mapping found
      if (str_eq(name, frame_var->name) && type == frame_var->type)
      {
        printf("Found!\n");
        return frame_var;
      }
      // Still looking
      frame_var = frame_var->next;
    }
    printf("Not found, attempting to look in parent frame!\n");
    printf("Current frame: \n");
    print_frame(frame);

    frame = frame->parent;
  }

  printf("Variable lookup failed!\n");

  return NULL;
}

/*
 * Returns new env sruct
 *
 */
ENV *new_env(char *name, int type, STATE *state)
{
  // Init env mapping
  ENV *new_env = malloc(sizeof(ENV));
  int name_length = strlen(name);

  new_env->name = malloc(name_length * sizeof(char));
  strcpy(new_env->name, name);
  new_env->type = type;
  new_env->state = state;
  new_env->is_return = false;

  printf("New var created: Name: %s, var->name: \"%s\"\n", name, new_env->name);

  return new_env;
}

STATE *new_int_state(int value)
{
  STATE *state = malloc(sizeof(STATE));
  state->value = value;
  return state;
}

STATE *new_fn_state(function *function)
{
  STATE *state = malloc(sizeof(STATE));
  state->function = function;
  return state;
}

STATE *new_var_name_state(char *name)
{
  STATE *state = malloc(sizeof(STATE));
  int name_length = strlen(name);

  state->var_name = malloc(name_length * sizeof(char));
  strcpy(state->var_name, name);
  return state;
}

STATE *new_fn_body_state(NODE *body)
{
  STATE *state = malloc(sizeof(STATE));
  state->fn_body = body;
  return state;
}

STATE *new_param_state(PARAM *param)
{
  STATE *state = malloc(sizeof(STATE));
  state->param = param;
  return state;
}

STATE *new_env_state(ENV *env)
{
  STATE *state = malloc(sizeof(STATE));
  state->env = env;
  return state;
}

/* Attempts to store new variable, initialised to default values */
ENV *init_var(char *name, int type, FRAME *frame)
{
  // Check for collision
  if (lookup_var(name, type, frame))
  {
    printf("Attempted to initialise variable twice!\n");
    abort();
  }

  // Init union type (the state)
  STATE *new_state = NULL;

  if (type == INT_TYPE)
  {
    new_state = new_int_state(0);
  }
  else if (type == FN_TYPE)
  {
    new_state = new_fn_state( new_function(0, NULL, NULL, NULL) );
  }

  // Init env mapping
  ENV *new_var = new_env(name, type, new_state);
  _add_env(new_var, frame);

  return new_var;
}

/* Sets the state of the variable */
ENV *assign_var(char *name, int type, STATE* value, FRAME *frame)
{
  ENV *var = lookup_var(name, type, frame);

  if (var && var->type == type)
  {
    var->state = value;
    if (type == INT_TYPE)
    {
      printf("Assigned %d to variable \"%s\"!\n", value->value, name);
    }
  }
  else
  {
    printf("Error: Assigning to uninitialised variable, check type!\n");
    abort();
  }
  return var;
}

/*
 * Create new frame and attack it correctly to its parent
 *
 */
FRAME *new_frame(FRAME *parent, char *proc_id)
{
  FRAME *new_frame = malloc(sizeof(FRAME));
  new_frame->parent = parent;

  // Add frame as child of parent
  if (parent)
  {
    FRAME *child = parent->child;
    parent->child = new_frame;
    new_frame->sibling = child;
  }
  new_frame->symbols = new_symbtable();
  new_frame->proc_id = proc_id; // Should be tokens lexeme
  return new_frame;
}

/* Init some globals, or whatever else */
void init_environment(void)
{
  gbl_frame = new_frame(NULL, "gbl_frame");
}

VARIABLE *new_var(int type, TOKEN *t, FRAME *frame) {
  STATE *s;
  switch(type) {
   case INT_TYPE:
    s = new_int_state(0);
    break;
   case FN_TYPE:
    s = new_fn_state(new_function(INT_TYPE, frame, NULL, t->lexeme));
    break;
   default:
    printf("Error: type for new variable not recognised\n");
    exit(-1);
  }

  VARIABLE *var = malloc(sizeof(VARIABLE));
  var->type = type;
  var->state = s;
  return var;
}