#ifndef TYPES_H
#define TYPES_H

typedef enum
{
    T_NUM, // Number
    T_ERR, // Error
    T_SYM, // Symbol
    T_STR, // String
    T_EXP, // Expression
    T_LST, // List
    T_FUN  // Function
} val_t;

struct val;
union val_data;
struct env;
typedef struct val val;
typedef union val_data val_data;
typedef struct env env;

typedef val *(*builtin)(env *, val *);

union val_data
{
    double num;
    
    char *str;

    struct
    {
        builtin blt;
        env *env;
        val *header;
        val *body;
    } fun;
    
    struct
    {
        int count;
        val **list;
    } exp;
};

struct val
{
    val_t type;

    val_data d;
};

struct env
{
    env *parent;

    int count;
    char **keys;
    val **vals;
};

// ---------- Constructors ----------

val *new_num(double n);

val *new_err(char *format, ...);

val *new_sym(char *s);

val *new_str(char *s);

val *new_exp(void);

val *new_lst(void);

val *new_builtin_fun(builtin blt);

val *new_fun(val *header, val *body);

env *new_env(void);

// ---------- Destructors ----------

void free_val(val *v);

void free_env(env *e);

// ---------- Copy ----------

val *copy_val(val *v);

env *copy_env(env *e);

// ---------- Comparison ----------

int val_eq(val *x, val *y);

// ---------- Environment - Get, Set ----------

val *env_get(env *e, val *key);

void env_set(env *e, val *key, val *v);

void env_set_global(env *e, val *key, val *v);

// ---------- Print ----------

void print_val(val *v);

void print_str(val *v);

void print_exp(val *v, char start, char end);

void print_val_ln(val *v);

// ---------- Val - Type Name ----------

char *type_name(val *v);

// ---------- Expression/List - Add, Pop, Take, Join ----------

val *exp_add(val *v, val *child);

val *exp_pop(val *v, int i);

val *exp_take(val *v, int i);

val *exp_join(val *x, val *y);

// ---------- Function - Call ----------

val *call(env *e, val *first, val *v);

// ---------- Evaluation ----------

val *eval(env *e, val *v);

val *eval_exp(env *e, val *v);

#endif