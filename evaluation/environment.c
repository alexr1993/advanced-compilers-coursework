#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "environment.h"

// Environment maps names to locations
ENV *environment;

// State maps locations to values
STATE *state;

/* Adds env mapping to the environment */
ENV *_add_env(ENV *env)
{
    ENV *current = environment;

    /* Lazily initialise */
    if (!current)
    {
        // env is the first variable
        printf("Setting environment\n");
        environment = env;
        printf("environment->name: %s\n", environment->name);
    }
    else
    {
        // Add new env to the end
        while (current->next)
        {
            current = current->next;
        }
        current->next = env;
    }
    // Return whole environment
    return environment;
}

/* Finds and returns the env mapping with the given name and type */
ENV *lookup_var(char *name, int type)
{
    printf("Name: %s\n", name);
    printf("Checking there is any env\n");
    // Check there are any variables
    if (!environment || !environment->name)
    {
        return NULL;
    }
    // FIXME reading environment->name causes a segfault
    printf("Env->name: %s\n", environment->name);
    ENV *current_env = environment;

    /* Find the env mapping with the given name */
    while (true)
    {
        // Mapping found
        if (str_eq(name, current_env->name))
        {
            break;
        }
        // Lookup failed
        if (!current_env->next)
        {
            printf("Variable name lookup failed!\n");
            abort();
        }
        // Still looking
        else
        {
            current_env = current_env->next;
        }
    }
    return current_env;
}

ENV *new_env(char *name, int type, STATE *state)
{
    // Init env mapping
    ENV *new_env = malloc(sizeof(ENV));
    int name_length = strlen(name);

    new_env->name = malloc(name_length * sizeof(char));
    strncpy(new_env->name, name, name_length);
    printf("Name: %s, var->name: %s\n", name, new_env->name);
    new_env->type = type;
    new_env->state = state;

    return new_env;
}

/* Attempts to store new variable, initialised to default values */
ENV *init_var(char *name, int type)
{
    // Check for collision
    if (lookup_var(name, type))
    {
        printf("Attempted to initialise variable twice!\n");
        abort();
    }

    // Init union type (the state)
    STATE *new_state;

    if (type == INT_TYPE)
    {
        new_state = new_int_state(0);
    }
    else if (type == FN_TYPE)
    {
        new_state = new_fn_state( make_leaf(NULL) );
    }

    // Init env mapping
    ENV *new_var = new_env(name, type, new_state);
    _add_env(new_var);

    return new_var;
}

/* Sets the state of the variable */
ENV *assign_var(char *name, int type, STATE* value)
{
    ENV *var = lookup_var(name, type);

    if (var && var->type == type)
    {
        var->state = value;
    }
    else
    {
        printf("Error: Assigning to uninitialised variable, check type!");
        abort();
    }
}

STATE *new_int_state(int value)
{
    STATE *state = malloc(sizeof(STATE));
    state->value = value;
    return state;
}

STATE *new_fn_state(NODE *closure)
{
    STATE *state = malloc(sizeof(STATE));
    state->closure = closure;
    return state;
}

/* Init some globals, or whatever else */
void init_environment(void)
{
    ENV *var1 = init_var("testvar", INT_TYPE);

    STATE *state = new_int_state(42);
    assign_var("testvar", INT_TYPE, state);
}
