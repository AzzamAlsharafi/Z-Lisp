#ifndef BUILTIN_H
#define BUILTIN_H

#include "types.h"

val *b_head(env *e, val *v);

val *b_tail(env *e, val *v);

val *b_list(env *e, val *v);

val *b_eval(env *e, val *v);

val *exp_join(val *x, val *y);

val *b_len(env *e, val *v);

int check_reserved(char* sym);

val *def_var(env *e, val *v, char *op);

val *b_def(env *e, val *v);

val *b_put(env *e, val *v);

val *b_env(env *e, val *v);

val *b_exit(env *e, val *v);

val *b_fun(env *e, val *v);

val *b_if(env *e, val *v);

val *b_load(env *e, val *v);

val *b_print(env *e, val *v);

val *b_error(env *e, val *v);

val *compare(env *e, val *v, char *op);

val *b_eq(env *e, val *v);

val *b_neq(env *e, val *v);

val *num_operation(val *v, char *op);

val *num_math(val* v, char* op);

val *num_compare(val *v, char *op);

val *join(val *v);

val *str_concat(val *v);

val *b_add(env *e, val *v);

val *b_sub(env *e, val *v);

val *b_mul(env *e, val *v);

val *b_div(env *e, val *v);

val *b_mod(env *e, val *v);

val *b_pow(env *e, val *v);

val *b_min(env *e, val *v);

val *b_max(env *e, val *v);

val *b_gt(env *e, val *v);

val *b_lt(env *e, val *v);

void add_builtin(env *e, char *key, builtin blt);

void add_builtins(env *e);

char *builtin_name(builtin f);

#endif
