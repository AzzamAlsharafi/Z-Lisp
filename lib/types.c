#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "mpc.h"

#include "types.h"
#include "builtin.h"

// ---------- Constructors ---------- 

val *new_num(double n)
{
    val *v = malloc(sizeof(val));
    *v = (val){.type = T_NUM, .d.num = n};
    return v;
}

// Similar to printf formatting.
val *new_err(char *format, ...)
{
    va_list list;
    va_start(list, format);

    val *v = malloc(sizeof(val));
    *v = (val){.type = T_ERR, .d.str = malloc(512)};

    vsnprintf(v->d.str, 511, format, list);
    v->d.str = realloc(v->d.str, strlen(v->d.str) + 1);

    va_end(list);

    return v;
}

val *new_sym(char *s)
{
    val *v = malloc(sizeof(val));
    *v = (val){.type = T_SYM, .d.str = malloc(strlen(s) + 1)};
    strcpy(v->d.str, s);
    return v;
}

val *new_str(char *s)
{
    val *v = malloc(sizeof(val));
    *v = (val){.type = T_STR, .d.str = malloc(strlen(s) + 1)};
    strcpy(v->d.str, s);
    return v;
}

val *new_exp(void)
{
    val *v = malloc(sizeof(val));
    *v = (val){.type = T_EXP, .d.exp.count = 0, .d.exp.list = NULL};
    return v;
}

val *new_lst(void)
{
    val *v = new_exp();
    v->type = T_LST;
    return v;
}

val *new_builtin_fun(builtin blt)
{
    val *v = malloc(sizeof(val));
    *v = (val){.type = T_FUN, .d.fun.blt = blt};
    return v;
}

val *new_fun(val *header, val *body)
{
    val *v = malloc(sizeof(val));
    env *e = new_env();
    *v = (val){.type = T_FUN, .d.fun.blt = NULL, .d.fun.env = e, .d.fun.header = header, .d.fun.body = body};
    return v;
}

env *new_env(void)
{
    env *e = malloc(sizeof(env));

    e->parent = NULL;
    e->count = 0;
    e->keys = NULL;
    e->vals = NULL;

    return e;
}

// ---------- Destructors ----------

void free_val(val *v)
{

    switch (v->type)
    {
    case T_NUM:
        break;

    case T_FUN:
        if (!v->d.fun.blt)
        {
            free_val(v->d.fun.header);
            free_val(v->d.fun.body);
            free_env(v->d.fun.env);
        }
        break;

    case T_ERR:
        free(v->d.str);
        break;
    case T_SYM:
        free(v->d.str);
        break;

    case T_STR:
        free(v->d.str);
        break;

    case T_EXP:
    case T_LST:
        for (int i = 0; i < v->d.exp.count; i++)
        {
            free_val(v->d.exp.list[i]);
        }
        free(v->d.exp.list);
        break;
    }

    free(v);
}

void free_env(env *e)
{
    for (int i = 0; i < e->count; i++)
    {
        free(e->keys[i]);
        free_val(e->vals[i]);
    }
    free(e->keys);
    free(e->vals);
    free(e);
}

// ---------- Copy ----------

val *copy_val(val *v)
{
    val *c = malloc(sizeof(val));
    c->type = v->type;

    switch (v->type)
    {
    case T_NUM:
        c->d.num = v->d.num;
        break;

    case T_FUN:
        if (v->d.fun.blt)
        {
            c->d.fun.blt = v->d.fun.blt;
        }
        else
        {
            c->d.fun.blt = NULL;
            c->d.fun.env = copy_env(v->d.fun.env);
            c->d.fun.header = copy_val(v->d.fun.header);
            c->d.fun.body = copy_val(v->d.fun.body);
        }
        break;

    case T_ERR:
        c->d.str = malloc(strlen(v->d.str) + 1);
        strcpy(c->d.str, v->d.str);
        break;

    case T_SYM:
        c->d.str = malloc(strlen(v->d.str) + 1);
        strcpy(c->d.str, v->d.str);
        break;
    case T_STR:
        c->d.str = malloc(strlen(v->d.str) + 1);
        strcpy(c->d.str, v->d.str);
        break;

    case T_EXP:
    case T_LST:
        c->d.exp.count = v->d.exp.count;
        c->d.exp.list = malloc(sizeof(val *) * c->d.exp.count);
        for (int i = 0; i < c->d.exp.count; i++)
        {
            c->d.exp.list[i] = copy_val(v->d.exp.list[i]);
        }
        break;
    }

    return c;
}

env *copy_env(env *e)
{
    env *c = new_env();

    c->parent = e->parent;

    for (int i = 0; i < e->count; i++)
    {
        env_set(c, new_sym(e->keys[i]), e->vals[i]);
    }

    return c;
}

// ---------- Comparison ----------

int val_eq(val *x, val *y)
{
    if (x->type != y->type)
    {
        return 0;
    }

    switch (x->type)
    {
    case T_NUM:
        return (x->d.num == y->d.num);

    case T_ERR:
        return (strcmp(x->d.str, y->d.str) == 0);
    case T_SYM:
        return (strcmp(x->d.str, y->d.str) == 0);
    case T_STR:
        return (strcmp(x->d.str, y->d.str) == 0);

    case T_FUN:
        if (x->d.fun.blt || y->d.fun.blt)
        {
            return x->d.fun.blt == y->d.fun.blt;
        }
        else
        {
            return val_eq(x->d.fun.header, y->d.fun.header) && val_eq(x->d.fun.body, y->d.fun.body);
        }

    case T_LST:
    case T_EXP:
        if (x->d.exp.count != y->d.exp.count)
        {
            return 0;
        }
        for (int i = 0; i < x->d.exp.count; i++)
        {
            if (!val_eq(x->d.exp.list[i], y->d.exp.list[i]))
            {
                return 0;
            }
        }
        return 1;
        break;
    }
    return 0;
}

// ---------- Environment - Get, Set ----------

val *env_get(env *e, val *key)
{
    for (int i = 0; i < e->count; i++)
    {
        if (strcmp(e->keys[i], key->d.str) == 0)
        {
            return copy_val(e->vals[i]);
        }
    }

    if (e->parent)
    {
        return env_get(e->parent, key);
    }
    else
    {
        return new_err("Unknown symbol '%s'.", key->d.str);
    }
}

void env_set(env *e, val *key, val *v)
{
    for (int i = 0; i < e->count; i++)
    {
        if (strcmp(e->keys[i], key->d.str) == 0)
        {
            free_val(e->vals[i]);
            e->vals[i] = copy_val(v);
            return;
        }
    }

    e->count++;
    e->keys = realloc(e->keys, e->count * sizeof(char *));
    e->vals = realloc(e->vals, e->count * sizeof(val *));

    e->keys[e->count - 1] = malloc(strlen(key->d.str) + 1);
    strcpy(e->keys[e->count - 1], key->d.str);

    e->vals[e->count - 1] = copy_val(v);
}

// Set on the most parent environment.
void env_set_global(env *e, val *key, val *v)
{
    while (e->parent)
    {
        e = e->parent;
    }
    env_set(e, key, v);
}

// ---------- Print ----------

void print_val(val *v)
{
    switch (v->type)
    {
    case T_NUM:
        printf("%f", v->d.num);
        break;
    case T_ERR:
        printf("Error: %s", v->d.str);
        break;
    case T_SYM:
        printf("%s", v->d.str);
        break;
    case T_STR:
        print_str(v);
        break;
    case T_EXP:
        print_exp(v, '(', ')');
        break;
    case T_LST:
        print_exp(v, '{', '}');
        break;
    case T_FUN:
        if (v->d.fun.blt)
        {
            printf("<%s>", builtin_name(v->d.fun.blt));
        }
        else
        {
            printf("(fun ");
            print_val(v->d.fun.header);
            putchar(' ');
            print_val(v->d.fun.body);
            putchar(')');
        }
        break;
    }
}

void print_str(val *v)
{
    char *escaped = malloc(strlen(v->d.str) + 1);
    strcpy(escaped, v->d.str);

    escaped = mpcf_escape(escaped);

    printf("\"%s\"", escaped);

    free(escaped);
}

void print_exp(val *v, char start, char end)
{
    putchar(start);
    for (int i = 0; i < v->d.exp.count; i++)
    {
        print_val(v->d.exp.list[i]);

        if (i != (v->d.exp.count - 1))
        {
            putchar(' ');
        }
    }
    putchar(end);
}

// Print with newline.
void print_val_ln(val *v)
{
    print_val(v);
    putchar('\n');
}

// ---------- Val - Type Name ----------

char *type_name(val *v)
{
    switch (v->type)
    {
    case T_NUM:
        return "Number";
    case T_ERR:
        return "Error";
    case T_SYM:
        return "Symbol";
    case T_STR:
        return "String";
    case T_EXP:
        return "Expression";
    case T_LST:
        return "List";
    case T_FUN:
        return "Function";
    default:
        return "Unknown";
    }
}

// ---------- Expression/List - Add, Pop, Take, Join ----------

val *exp_add(val *v, val *child)
{
    v->d.exp.count++;
    v->d.exp.list = realloc(v->d.exp.list, sizeof(val *) * v->d.exp.count);
    v->d.exp.list[v->d.exp.count - 1] = child;
    return v;
}

val *exp_pop(val *v, int i)
{
    val *x = v->d.exp.list[i];

    memmove(&v->d.exp.list[i], &v->d.exp.list[i + 1], sizeof(val *) * (v->d.exp.count - i - 1));

    v->d.exp.count--;

    v->d.exp.list = realloc(v->d.exp.list, sizeof(val *) * v->d.exp.count);

    return x;
}

val *exp_take(val *v, int i)
{
    val *x = exp_pop(v, i);

    free_val(v);

    return x;
}

val *exp_join(val *x, val *y)
{
    while (y->d.exp.count)
    {
        exp_add(x, exp_pop(y, 0));
    }

    free_val(y);
    return x;
}

// ---------- Function - Call ----------

val *call(env *e, val *first, val *v)
{
    // If builtin function, call it directly.
    if (first->d.fun.blt)
    {
        return first->d.fun.blt(e, v);
    }

    // Number of arguments given, and number of function parameters.
    int given = v->d.exp.count;
    int total = first->d.fun.header->d.exp.count;

    // Consume arguments.
    while (v->d.exp.count)
    {
        // If arguments are more than parameters, throw error.
        if (first->d.fun.header->d.exp.count == 0)
        {
            free_val(v);
            return new_err("Function received too many arguements. Received %i. Expected %i.", given, total);
        }

        // Pop symbol (parameter).
        val *sym = exp_pop(first->d.fun.header, 0);

        // Special symbol '&'. Symbol that comes after '&', will contain all remaining arguments in a list.
        if (strcmp(sym->d.str, "&") == 0)
        {
            // Ensure '&' is followed by exactly one Symbol.
            if (first->d.fun.header->d.exp.count != 1)
            {
                free_val(v);
                return new_err("Invalid function format. Symbol '&' should be followed by exactly one Symbol.");
            }

            // Store remaining arguments in symbol following '&'.
            val *next_sym = exp_pop(first->d.fun.header, 0);
            env_set(first->d.fun.env, next_sym, b_list(e, v));
            free_val(sym);
            free_val(next_sym);
            break;
        }

        // Store argument with symbol in function's local environment.
        val *val = exp_pop(v, 0);
        env_set(first->d.fun.env, sym, val);
        free_val(sym);
        free_val(val);
    }

    free_val(v);

    // If '&' is the first remainig symbol in the function header, and no arguments are given, store empty list.
    if (first->d.fun.header->d.exp.count > 0 && strcmp(first->d.fun.header->d.exp.list[0]->d.str, "0") == 0)
    {
        // Ensure '&' is followed by exactly one Symbol.
        if (first->d.fun.header->d.exp.count != 2)
        {
            return new_err("Invalid function format. Symbol '&' should be followed by exactly one Symbol.");
        }

        free_val(exp_pop(first->d.fun.header, 0));

        // Store empty list in the symbol following '&'.
        val *sym = exp_pop(first->d.fun.header, 0);
        val *val = new_lst();
        env_set(first->d.fun.env, sym, val);
        free_val(sym);
        free_val(val);
    }

    // If all parameters are filled, evaluate the function.
    if (first->d.fun.header->d.exp.count == 0)
    {
        first->d.fun.env->parent = e;

        return b_eval(first->d.fun.env, exp_add(new_exp(), copy_val(first->d.fun.body)));
    }
    else // Otherwise, return new function with remaining parameters.
    {
        return copy_val(first);
    }
}

// ---------- Evaluation ---------- 

val *eval(env *e, val *v)
{
    // If Symbol, get value from environment.
    if (v->type == T_SYM)
    {
        val *s = env_get(e, v);
        free_val(v);
        return s;
    }

    // If Expression, evaluate.
    if (v->type == T_EXP)
    {
        return eval_exp(e, v);
    }

    // All other types remain the same.
    return v;
}

val *eval_exp(env *e, val *v)
{   
    // Evaluate all children.
    for (int i = 0; i < v->d.exp.count; i++)
    {
        v->d.exp.list[i] = eval(e, v->d.exp.list[i]);
    }

    // If any child is an error, return it.
    for (int i = 0; i < v->d.exp.count; i++)
    {
        if (v->d.exp.list[i]->type == T_ERR)
        {
            return exp_take(v, i);
        }
    }

    // If empty expression, return it.
    if (v->d.exp.count == 0)
    {
        return v;
    }

    // If single child expression, return child.
    if (v->d.exp.count == 1)
    {
        return exp_take(v, 0);
    }
    
    // Ensure first child is a function.
    val *first = exp_pop(v, 0);
    if (first->type != T_FUN)
    {
        val *err = new_err("Expression must start with a Function. Received '%s'.", type_name(first));
        free_val(v);
        free_val(first);
        return err;
    }

    // Call function with remaining children.
    val *result = call(e, first, v);
    free_val(first);
    return result;
}