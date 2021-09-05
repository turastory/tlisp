#include <stdio.h>
#include <stdlib.h>

/* If we are compiling on Windows compile these functions */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char *readline(char *prompt)
{
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char *cpy = malloc(strlen(buffer) + 1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy) - 1] = '\0';
  return cpy;
}

/* Fake add_history function */
void add_history(char *unused) {}

/* Otherwise include the editline headers */
#else
#include <editline/readline.h>
#endif

#include "mpc.h"

int main(int argc, char **argv)
{
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
            " \
    number   : /-?[0-9]+(\\.[0-9]+)?/ ;                   \
    operator : '+' | '-' | '*' | '/' | '%' |            \
    \"add\" | \"sub\" | \"mul\" | \"div\" | \"rem\";    \
    expr     : <number> | '(' <expr> <operator> <expr> ')' ;  \
    lispy    : /^/ <expr> <operator> <expr> /$/ ;             \
  ",
            Number, Operator, Expr, Lispy);

  puts("tlisp version 0.0.1");
  puts("Press Ctrl+C to exit.");

  while (1)
  {
    // fputs("tlisp> ", stdout);
    char *input = readline("tlisp> ");
    add_history(input);

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r))
    {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    }
    else
    {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  mpc_cleanup(4, Number, Operator, Expr, Lispy);

  return 0;
}