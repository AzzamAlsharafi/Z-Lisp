#include <limits.h>
#include <float.h>

#include "mpc.h"

#include "types.h"
#include "parser.h"

// Parse input, evalute, and return val result.
val *parse(char *input, mpc_parser_t *parser, env *e)
{
    mpc_result_t r;

    if (mpc_parse("<stdin>", input, parser, &r))
    {
        mpc_ast_t *node = r.output;
        // mpc_ast_print(node);

        val *v = eval(e, parse_node(node));
        mpc_ast_delete(node);

        return v;
    }
    else
    {
        val *err = new_err("Parser Error: %s", r.error);
        mpc_err_delete(r.error);
        return err;
    }
}

// Parse AST node and return val.
val *parse_node(mpc_ast_t *node)
{
    if (strstr(node->tag, "number"))
    {
        return parse_num(node);
    }
    if (strstr(node->tag, "symbol"))
    {
        return new_sym(node->contents);
    }
    if (strstr(node->tag, "string"))
    {
        return parse_str(node);
    }

    // If neither, must be root, Expression, or List.
    val *v = NULL;
    if (strcmp(node->tag, ">") == 0 || strstr(node->tag, "expression"))
    {
        v = new_exp();
    }
    else if (strstr(node->tag, "list"))
    {
        v = new_lst();
    }

    if (v != NULL)
    {
        // Parse children.
        for (int i = 0; i < node->children_num; i++)
        {
            // Ignore Comments, regex, and parentheses/braces nodes.
            if (strcmp(node->children[i]->contents, "(") == 0 || strcmp(node->children[i]->contents, ")") == 0 || strcmp(node->children[i]->contents, "{") == 0 || strcmp(node->children[i]->contents, "}") == 0 || strcmp(node->children[i]->tag, "regex") == 0 || strstr(node->children[i]->tag, "comment"))
            {
                continue;
            }

            v = exp_add(v, parse_node(node->children[i]));
        }
    }

    return v;
}

// Parse number node and return val.
val *parse_num(mpc_ast_t *node)
{
    errno = 0;

    if (strstr(node->contents, "."))
    {
        return string_to_float(node->contents);
    }
    else
    {
        return string_to_int(node->contents);
    }
}

// Parse string node and return val.
val *parse_str(mpc_ast_t *node)
{
    node->contents[strlen(node->contents) - 1] = '\0';

    char *unescaped = malloc(strlen(node->contents + 1) + 1);
    strcpy(unescaped, node->contents + 1);

    unescaped = mpcf_unescape(unescaped);

    val *str = new_str(unescaped);

    free(unescaped);

    return str;
}

val *string_to_int(char *str)
{
    errno = 0;
    char *end = NULL;

    long n = strtol(str, &end, 10);

    val* v = NULL;

    if (str == end) // No digits found.
        v = new_err("Invalid Integer '%s'. No digits found.", str);
    else if (errno == ERANGE && n == LONG_MIN) // Underflow.
        v = new_err("Invalid Integer '%s'. Underflow.", str);
    else if (errno == ERANGE && n == LONG_MAX) // Overflow.
        v = new_err("Invalid Integer '%s'. Overflow.", str);
    else if (errno != 0 && n == 0) // Unspecified error.
        v = new_err("Invalid Integer '%s'.", str);
    else if (errno == 0 && str && !*end) // Valid.
        v = new_int(n);
    else if (errno == 0 && str && *end != 0) // Valid but additional characters remains.
        v = new_err("Invalid Integer '%s'. Additional characters found.", str);
    
    return v;
}

val *string_to_float(char *str)
{
    errno = 0;
    char *end = NULL;

    double n = strtod(str, &end);

    val* v = NULL;

    if (str == end) // No digits found.
        v = new_err("Invalid Float '%s'. No digits found.", str);
    else if (errno == ERANGE && n == DBL_MIN) // Underflow.
        v = new_err("Invalid Float '%s'. Underflow.", str);
    else if (errno == ERANGE && n == DBL_MAX) // Overflow.
        v = new_err("Invalid Float '%s'. Overflow.", str);
    else if (errno != 0 && n == 0) // Unspecified error.
        v = new_err("Invalid Float '%s'.", str);
    else if (errno == 0 && str && !*end) // Valid.
        v = new_flt(n);
    else if (errno == 0 && str && *end != 0) // Valid but additional characters remains.
        v = new_err("Invalid Float '%s'. Additional characters found.", str);
    
    return v;
}