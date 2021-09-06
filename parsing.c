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
#include "math.h"

typedef struct
{
  int type;
  long num;
  int err;
} lval;

enum
{
  LVAL_NUM,
  LVAL_ERR
};

enum
{
  LERR_DIV_ZERO,
  LERR_BAD_OP,
  LERR_BAD_NUM
};

lval lval_num(long x)
{
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

lval lval_err(int x)
{
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

void lval_print(lval v)
{
  switch (v.type)
  {
  case LVAL_NUM:
    printf("%li", v.num);
    break;
  case LVAL_ERR:
    if (v.err == LERR_DIV_ZERO)
    {
      printf("Error: Divided by zero");
    }
    if (v.err == LERR_BAD_OP)
    {
      printf("Error: Invalid operator");
    }
    if (v.err == LERR_BAD_NUM)
    {
      printf("Error: Invalid number");
    }
    break;
  }
}

void lval_println(lval v)
{
  lval_print(v);
  putchar('\n');
}

long eval_op(char *op, long x, long y)
{
  if (strcmp(op, "+") == 0 || strcmp(op, "add") == 0)
    return x + y;
  if (strcmp(op, "-") == 0 || strcmp(op, "sub") == 0)
    return x - y;
  if (strcmp(op, "*") == 0 || strcmp(op, "mul") == 0)
    return x * y;
  if (strcmp(op, "/") == 0 || strcmp(op, "div") == 0)
    return x / y;
  if (strcmp(op, "%") == 0 || strcmp(op, "rem") == 0)
    return x % y;
  if (strcmp(op, "^") == 0)
    return pow(x, y);
  return 0;
}

long eval(mpc_ast_t *t)
{
  // Base case
  if (strstr(t->tag, "number"))
  {
    return atoi(t->contents);
  }

  // The operator is always on third position.
  char *op = t->children[1]->contents;

  long x = eval(t->children[2]);
  int i = 3;
  while (strstr(t->children[i]->tag, "expr"))
  {
    x = eval_op(op, x, eval(t->children[i]));
    i++;
  }

  return x;
}

int main(int argc, char **argv)
{
  mpc_parser_t *Number = mpc_new("number");
  mpc_parser_t *Operator = mpc_new("operator");
  mpc_parser_t *Expr = mpc_new("expr");
  mpc_parser_t *Lispy = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
            " \
    number   : /-?[0-9]+(\\.[0-9]+)?/ ;                 \
    operator : '+' | '-' | '*' | '/' | '%' | '^' |      \
    \"add\" | \"sub\" | \"mul\" | \"div\" | \"rem\";    \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    lispy    : /^/ <operator> <expr>+ /$/ ;             \
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
      long result = eval(r.output);
      printf("%li\n", result);
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