#include <stdlib.h>

#include "mpc.h"

#include "builtin.h"
#include "types.h"
#include "parser.h"

// Assert condition is true, otherwise return error and free argument.
#define ASSERT(args, cond, format, ...)            \
    if (!(cond))                                   \
    {                                              \
        val *err = new_err(format, ##__VA_ARGS__); \
        free_val(args);                            \
        return err;                                \
    }

extern mpc_parser_t *Parser;

// Return the first element of a List.
val *b_head(env *e, val *v)
{
    ASSERT(v, v->count == 1, "Function 'head' recieved %i arguments. Expected 1 argument.", v->count)
    ASSERT(v, v->list[0]->type == T_LST, "Function 'head' recieved '%s'. Expected List.", type_name(v->list[0]))
    ASSERT(v, v->list[0]->count > 0, "Function 'head' recieved {}.")

    val *l = exp_take(v, 0);
    while (l->count > 1)
    {
        free_val(exp_pop(l, 1));
    }
    return l;
}

// Return the tail (all elements except the first) of a List.
val *b_tail(env *e, val *v)
{
    ASSERT(v, v->count == 1, "Function 'tail' recieved %i arguments. Expected 1 argument.", v->count)
    ASSERT(v, v->list[0]->type == T_LST, "Function 'tail' recieved '%s'. Expected List.", type_name(v->list[0]))
    ASSERT(v, v->list[0]->count > 0, "Function 'tail' recieved {}.")

    val *l = exp_take(v, 0);
    free_val(exp_pop(l, 0));
    return l;
}

// Converts arguments to a List.
val *b_list(env *e, val *v)
{
    v->type = T_LST;
    return v;
}

// Evaluate the a List as an Expression.
val *b_eval(env *e, val *v)
{
    ASSERT(v, v->count == 1, "Function 'eval' recieved %i arguments. Expected 1 argument.", v->count)
    ASSERT(v, v->list[0]->type == T_LST, "Function 'eval' recieved '%s'. Expected List.", type_name(v->list[0]))

    val *l = exp_take(v, 0);
    l->type = T_EXP;
    return eval(e, l);
}

// Join two Lists.
val *b_join(env *e, val *v)
{
    for (int i = 0; i < v->count; i++)
    {
        ASSERT(v, v->list[i]->type == T_LST, "Function 'join' argument %i is '%s'. Expected List.", i, type_name(v->list[i]));
    }

    val *l = exp_pop(v, 0);

    while (v->count)
    {
        l = exp_join(l, exp_pop(v, 0));
    }

    free_val(v);
    return l;
}

// Return the length of a List.
val *b_len(env *e, val *v)
{
    ASSERT(v, v->count == 1, "Function 'len' recieved %i arguments. Expected 1 argument.", v->count);
    ASSERT(v, v->list[0]->type == T_LST, "Function 'len' recieved '%s'. Expected List.", type_name(v->list[0]));

    val *l = exp_take(v, 0);
    val *len = new_num(l->count);
    free_val(l);
    return len;
}

// Define a variable in an environment. Accepts a List of Symbols, followed by values.
// Accepts two operations: 'def' (global) and '=' (local).
val *def_var(env *e, val *v, char *op)
{
    ASSERT(v, v->list[0]->type == T_LST, "Function '%s' recieved '%s'. Expected List.", op, type_name(v->list[0]));

    val *keys = v->list[0];

    for (int i = 0; i < keys->count; i++)
    {
        ASSERT(v, keys->list[i]->type == T_SYM, "Function '%s' received '%s' at element %i. Expected Symbols only.", op, type_name(keys->list[i]), i);

        int condition = strcmp(keys->list[i]->sym, "==") == 0 || strcmp(keys->list[i]->sym, "!=") == 0 || strcmp(keys->list[i]->sym, "error") == 0 || strcmp(keys->list[i]->sym, "print") == 0 || strcmp(keys->list[i]->sym, "load") == 0 || strcmp(keys->list[i]->sym, "if") == 0 || strcmp(keys->list[i]->sym, "==") == 0 || strcmp(keys->list[i]->sym, "<") == 0 || strcmp(keys->list[i]->sym, ">") == 0 || strcmp(keys->list[i]->sym, "len") == 0 || strcmp(keys->list[i]->sym, "+") == 0 || strcmp(keys->list[i]->sym, "-") == 0 || strcmp(keys->list[i]->sym, "*") == 0 || strcmp(keys->list[i]->sym, "/") == 0 || strcmp(keys->list[i]->sym, "%") == 0 || strcmp(keys->list[i]->sym, "^") == 0 || strcmp(keys->list[i]->sym, "min") == 0 || strcmp(keys->list[i]->sym, "max") == 0 || strcmp(keys->list[i]->sym, "def") == 0 || strcmp(keys->list[i]->sym, "env") == 0 || strcmp(keys->list[i]->sym, "list") == 0 || strcmp(keys->list[i]->sym, "join") == 0 || strcmp(keys->list[i]->sym, "head") == 0 || strcmp(keys->list[i]->sym, "tail") == 0 || strcmp(keys->list[i]->sym, "eval") == 0 || strcmp(keys->list[i]->sym, "exit") == 0 || strcmp(keys->list[i]->sym, "fun") == 0 || strcmp(keys->list[i]->sym, "=") == 0;

        ASSERT(v, !condition, "Function '%s' received forbidden symbol '%s'. This is a builtin symbol.", op, keys->list[i]->sym);
    }

    ASSERT(v, keys->count == v->count - 1, "Function '%s' received unmatching number of Symbols (%i) and values (%i).", op, keys->count, v->count - 1);

    for (int i = 0; i < keys->count; i++)
    {
        if (strcmp(op, "def") == 0)
        {
            env_set_global(e, keys->list[i], v->list[i + 1]);
        }

        if (strcmp(op, "=") == 0)
        {
            env_set(e, keys->list[i], v->list[i + 1]);
        }
    }

    free_val(v);
    return new_exp();
}

// Global define.
val *b_def(env *e, val *v)
{
    return def_var(e, v, "def");
}

// Local define.
val *b_put(env *e, val *v)
{
    return def_var(e, v, "=");
}

// Return the environment as a List of key-value pairs.
val *b_env(env *e, val *v)
{
    ASSERT(v, v->count == 1, "Function 'env' recieved %i arguments. Expected 1 arguments.", v->count);
    ASSERT(v, v->list[0]->type == T_LST && v->list[0]->count == 0, "Function 'env' recieved '%s'. Expected {}.", type_name(v->list[0]));

    val *l = new_lst();

    for (int i = 0; i < e->count; i++)
    {
        val *item = new_lst();
        exp_add(item, new_sym(e->keys[i]));
        exp_add(item, env_get(e, new_sym(e->keys[i])));
        exp_add(l, item);
    }

    free_val(v);

    return l;
}

// Exit the program.
val *b_exit(env *e, val *v)
{
    ASSERT(v, v->count == 1, "Function 'exit' recieved %i arguments. Expected 1 arguments.", v->count);
    ASSERT(v, v->list[0]->type == T_LST && v->list[0]->count == 0, "Function 'exit' recieved '%s'. Expected {}.", type_name(v->list[0]));

    exit(EXIT_SUCCESS);
}

// Create a function. Accepts a List of Symbols as header, followed by a List as body.
val *b_fun(env *e, val *v)
{
    ASSERT(v, v->count == 2, "Function 'fun' recieved %i arguments. Expected 2 arguments.", v->count);

    for (int i = 0; i < v->count; i++)
    {
        ASSERT(v, v->list[i]->type == T_LST, "Function 'fun' argument %i is '%s'. Expected List.", i, type_name(v->list[i]));
    }

    for (int i = 0; i < v->list[0]->count; i++)
    {
        ASSERT(v, v->list[0]->list[i]->type == T_SYM, "Function header must contain Symbols only. Parameter %i is '%s'.",
               i, type_name(v->list[0]->list[i]));
    }

    val *header = exp_pop(v, 0);
    val *body = exp_pop(v, 0);
    free_val(v);

    return new_fun(header, body);
}

// If statement. Accepts a Number, and two Lists. Evaluate first if Number is true, otherwise evaluate second.
val *b_if(env *e, val *v)
{
    ASSERT(v, v->count == 3, "Function 'if' recieved %i arguments. Expected 3 arguments.", v->count);
    ASSERT(v, v->list[0]->type == T_NUM, "Function 'if' argument %i is '%s'. Expected Number.", 0, type_name(v->list[0]));
    ASSERT(v, v->list[1]->type == T_LST, "Function 'if' argument %i is '%s'. Expected List.", 1, type_name(v->list[0]));
    ASSERT(v, v->list[2]->type == T_LST, "Function 'if' argument %i is '%s'. Expected List.", 2, type_name(v->list[0]));

    if (v->list[0]->num)
    {
        return b_eval(e, exp_add(new_lst(), v->list[1]));
    }
    else
    {
        return b_eval(e, exp_add(new_lst(), v->list[2]));
    }
}

// Load/run a Z-Lisp file. Accepts a String as a file name. Returns () or Error if failed.
val *b_load(env *e, val *v)
{
    ASSERT(v, v->count == 1, "Function 'load' recieved %i arguments. Expected 1 argument.", v->count);
    ASSERT(v, v->list[0]->type == T_STR, "Function 'load' argument %i is '%s'. Expected String.", 0, type_name(v->list[0]));

    mpc_result_t r;
    if (mpc_parse_contents(v->list[0]->str, Parser, &r))
    {
        val *exp = parse_node(r.output);
        mpc_ast_delete(r.output);

        while (exp->count)
        {
            val *x = eval(e, exp_pop(exp, 0));

            if (x->type == T_ERR)
            {
                print_val_ln(x);
            }

            free_val(x);
        }

        free_val(exp);
        free_val(v);

        return new_exp();
    }
    else
    {
        char *err_msg = mpc_err_string(r.error);
        mpc_err_delete(r.error);

        val *err = new_err("Failed to load library: %s", err_msg);
        free(err_msg);
        free_val(v);

        return err;
    }
}

// Print arguments separated by a space, followed by a newline to stdout.
val *b_print(env *e, val *v)
{
    for (int i = 0; i < v->count; i++)
    {
        print_val(v->list[i]);
        putchar(' ');
    }

    putchar('\n');
    free_val(v);

    return new_exp();
}

// Throw an error. Accepts a String as an error message.
val *b_error(env *e, val *v)
{
    ASSERT(v, v->count == 1, "Function 'error' recieved %i arguments. Expected 1 argument.", v->count);
    ASSERT(v, v->list[0]->type == T_STR, "Function 'error' argument %i is '%s'. Expected String.", 0, type_name(v->list[0]));

    val *err = new_err(v->list[0]->str);

    free_val(v);
    return err;
}

// Compare two arguments.
// Accepts two operations: '==' and '!='.
val *compare(env *e, val *v, char *op)
{
    ASSERT(v, v->count == 2, "Function '%s' recieved %i arguments. Expected 2 argument.", op, v->count);

    int r;
    if (strcmp(op, "==") == 0)
    {
        r = val_eq(v->list[0], v->list[1]);
    }
    if (strcmp(op, "!=") == 0)
    {
        r = !val_eq(v->list[0], v->list[1]);
    }
    free_val(v);
    return new_num(r);
}

// Check if two arguments are equal.
val *b_eq(env *e, val *v)
{
    return compare(e, v, "==");
}

// Check if two arguments are not equal.
val *b_neq(env *e, val *v)
{
    return compare(e, v, "!=");
}

// Perform a mathematical operation on Number arguements. No limit on the number of arguments.
// Accepts operations: '+', '-', '*', '/', '%', '^', 'min', 'max', '>', '<'.
val *operation(env *e, val *v, char *op)
{
    for (int i = 0; i < v->count; i++)
    {
        if (v->list[i]->type != T_NUM)
        {
            val *err = new_err("Operator '%s' received incorrect arguments. Arguement %i is a '%s'.", op, i, type_name(v->list[i]));
            free_val(v);
            return err;
        }
    }

    val *x = exp_pop(v, 0);

    // Special case for unary minus
    if (strcmp(op, "-") == 0 && v->count == 0)
    {
        x->num = -x->num;
    }

    while (v->count > 0)
    {
        val *y = exp_pop(v, 0);

        if (strcmp(op, "+") == 0)
        {
            x->num += y->num;
        }
        else if (strcmp(op, "-") == 0)
        {
            x->num -= y->num;
        }
        else if (strcmp(op, "*") == 0)
        {
            x->num *= y->num;
        }
        else if (strcmp(op, "/") == 0)
        {
            if (y->num == 0)
            {
                free_val(x);
                free_val(y);
                x = new_err("Division By Zero.");
                break;
            }
            x->num /= y->num;
        }
        else if (strcmp(op, "%") == 0)
        {
            x->num = fmod(x->num, y->num);
        }
        else if (strcmp(op, "^") == 0)
        {
            x->num = pow(x->num, y->num);
        }
        else if (strcmp(op, "min") == 0)
        {
            x->num = x->num < y->num ? x->num : y->num;
        }
        else if (strcmp(op, "max") == 0)
        {
            x->num = x->num > y->num ? x->num : y->num;
        }
        else if (strcmp(op, ">") == 0)
        {
            x->num = x->num > y->num;
        }
        else if (strcmp(op, "<") == 0)
        {
            x->num = x->num < y->num;
        }

        free_val(y);
    }

    free_val(v);
    return x;
}

val *b_add(env *e, val *v)
{
    return operation(e, v, "+");
}

val *b_sub(env *e, val *v)
{
    return operation(e, v, "-");
}

val *b_mul(env *e, val *v)
{
    return operation(e, v, "*");
}

val *b_div(env *e, val *v)
{
    return operation(e, v, "/");
}

val *b_mod(env *e, val *v)
{
    return operation(e, v, "%");
}

val *b_pow(env *e, val *v)
{
    return operation(e, v, "^");
}

val *b_min(env *e, val *v)
{
    return operation(e, v, "min");
}

val *b_max(env *e, val *v)
{
    return operation(e, v, "max");
}

val *b_gt(env *e, val *v)
{
    return operation(e, v, ">");
}

val *b_lt(env *e, val *v)
{
    return operation(e, v, "<");
}

// Register a builtin function in the environment.
void add_builtin(env *e, char *key, builtin blt)
{
    val *sym = new_sym(key);
    val *val = new_builtin_fun(blt);
    env_set(e, sym, val);
    free_val(sym);
    free_val(val);
}

// Register all builtin functions in the environment.
void add_builtins(env *e)
{
    add_builtin(e, "list", b_list);
    add_builtin(e, "head", b_head);
    add_builtin(e, "tail", b_tail);
    add_builtin(e, "join", b_join);
    add_builtin(e, "eval", b_eval);
    add_builtin(e, "+", b_add);
    add_builtin(e, "-", b_sub);
    add_builtin(e, "*", b_mul);
    add_builtin(e, "/", b_div);
    add_builtin(e, "%", b_mod);
    add_builtin(e, "^", b_pow);
    add_builtin(e, "min", b_min);
    add_builtin(e, "max", b_max);
    add_builtin(e, "def", b_def);
    add_builtin(e, "=", b_put);
    add_builtin(e, "env", b_env);
    add_builtin(e, "exit", b_exit);
    add_builtin(e, "fun", b_fun);
    add_builtin(e, "len", b_len);
    add_builtin(e, ">", b_gt);
    add_builtin(e, "<", b_lt);
    add_builtin(e, "==", b_eq);
    add_builtin(e, "!=", b_neq);
    add_builtin(e, "if", b_if);
    add_builtin(e, "load", b_load);
    add_builtin(e, "print", b_print);
    add_builtin(e, "error", b_error);
}

// Return the name of a builtin function.
char *builtin_name(builtin f)
{
    if (f == b_list)
    {
        return "builtin_list";
    }
    if (f == b_head)
    {
        return "builtin_head";
    }
    if (f == b_tail)
    {
        return "builtin_tail";
    }
    if (f == b_join)
    {
        return "builtin_join";
    }
    if (f == b_eval)
    {
        return "builtin_eval";
    }
    if (f == b_add)
    {
        return "builtin_add";
    }
    if (f == b_sub)
    {
        return "builtin_sub";
    }
    if (f == b_mul)
    {
        return "builtin_mul";
    }
    if (f == b_div)
    {
        return "builtin_div";
    }
    if (f == b_mod)
    {
        return "builtin_mod";
    }
    if (f == b_pow)
    {
        return "builtin_pow";
    }
    if (f == b_min)
    {
        return "builtin_min";
    }
    if (f == b_max)
    {
        return "builtin_max";
    }
    if (f == b_def)
    {
        return "builtin_def";
    }
    if (f == b_put)
    {
        return "builtin_put";
    }
    if (f == b_env)
    {
        return "builtin_env";
    }
    if (f == b_exit)
    {
        return "builtin_exit";
    }
    if (f == b_fun)
    {
        return "builtin_fun";
    }
    if (f == b_len)
    {
        return "builtin_len";
    }
    if (f == b_gt)
    {
        return "builtin_gt";
    }
    if (f == b_lt)
    {
        return "builtin_lt";
    }
    if (f == b_eq)
    {
        return "builtin_eq";
    }
    if (f == b_neq)
    {
        return "builtin_neq";
    }
    if (f == b_if)
    {
        return "builtin_if";
    }
    if (f == b_load)
    {
        return "builtin_load";
    }
    if (f == b_print)
    {
        return "builtin_print";
    }
    if (f == b_error)
    {
        return "builtin_error";
    }

    return "builtin_function";
}