#ifndef PARSER_H
#define PARSER_H

#include "mpc.h"

#include "types.h"

val *parse(char *input, mpc_parser_t *parser, env *e);

val *parse_node(mpc_ast_t *node);

val *parse_num(mpc_ast_t *node);

val *parse_str(mpc_ast_t *node);

val *string_to_int(char *str);

val *string_to_float(char *str);

#endif