// gcc -std=c99 -Wall -Werror -o zlisp main.c mpc.c types.c builtin.c parser.c -ledit -lm

#define VERSION "0.0.0"

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
// Implementation for Windows, replacing editline/readline functions.
#include <string.h>

static char buffer[2048];

char *readline(char *prompt)
{
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char *cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

void add_history(char *unused) {}
#else

#include <editline/readline.h>

#endif

#include "lib/mpc.h"

#include "lib/types.h"
#include "lib/builtin.h"
#include "lib/parser.h"

// Define parsers variables.
mpc_parser_t *Number;
mpc_parser_t *String;
mpc_parser_t *Symbol;
mpc_parser_t *Expression;
mpc_parser_t *List;
mpc_parser_t *Component;
mpc_parser_t *Comment;
mpc_parser_t *Parser;

int main(int argc, char **argv)
{
    // Create parsers.
    Number = mpc_new("number");
    String = mpc_new("string");
    Symbol = mpc_new("symbol");
    Expression = mpc_new("expression");
    List = mpc_new("list");
    Component = mpc_new("component");
    Comment = mpc_new("comment");
    Parser = mpc_new("parser");

    // Define parsers rules.
    mpca_lang(MPCA_LANG_DEFAULT,
    "                                                                                       \
        number      : /-?[0-9]+\\.?[0-9]*/ ;                                                \
        string      : /\"(\\\\.|[^\"])*\"/ ;                                                \
        symbol      : /[a-zA-Z0-9^%_+\\-*\\/\\\\=<>!&]+/ ;                                  \
        expression  : '(' <component>* ')' ;                                                \
        list        : '{' <component>* '}' ;                                                \
        component   : <number> | <string> | <symbol> | <expression> | <list> | <comment> ;  \
        comment     : /;[^\\r\\n]*/ ;                                                       \
        parser      : /^/ <component>* /$/ ;                                                \
    ", Number, String, Symbol, Expression, List, Component, Comment, Parser);

    // Initialize global environment.
    env *e = new_env();
    add_builtins(e);

    // Load standard library.
    val *std = b_load(e, exp_add(new_exp(), new_str("std.zsp")));

    // Print error if occurred during loading.
    if (std->type == T_ERR)
    {
        print_val_ln(std);
    }

    free_val(std);

    // Check if arugments are passed,
    // Accepts file names as arguments, and load/run them sequentially, then exit.
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            val *args = exp_add(new_exp(), new_str(argv[i]));

            val *x = b_load(e, args);

            // Print error if any.
            if (x->type == T_ERR)
            {
                print_val_ln(x);
            }

            free_val(x);
        }
    }
    else // If no arguments are passed, run interactive prompt.
    {
        puts("Z-Lisp, v: " VERSION);
        puts("Press Ctrl-C to Exit\n");

        while (1)
        {
            char *input = readline("z-lisp> ");

            add_history(input);

            if (input[0] != '\0')
            {
                // Parse input, evalute, and return val result.
                val* x = parse(input, Parser, e);
                print_val_ln(x);
            }

            free(input);
        }
    }

    // Cleanup.
    free_env(e);
    mpc_cleanup(8, Number, String, Symbol, Expression, List, Component, Comment, Parser);
}
