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
    double n = strtod(node->contents, NULL);

    return errno != ERANGE ? new_num(n) : new_err("Invalid Number '%s'.", node->contents);
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
