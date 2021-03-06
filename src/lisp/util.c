#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "libstephen/lisp.h"

static lisp_list *lisp_new_pair_list(lisp_runtime *rt, lisp_value *one, lisp_value *two)
{
  lisp_list *first_node = (lisp_list*) lisp_new(rt, type_list);
  lisp_list *second_node = (lisp_list*) lisp_new(rt, type_list);
  first_node->left = one;
  first_node->right = (lisp_value*) second_node;
  second_node->left = two;
  second_node->right = lisp_nil_new(rt);
  return first_node;
}

void lisp_scope_bind(lisp_scope *scope, lisp_symbol *symbol, lisp_value *value)
{
  ht_insert(&scope->scope, PTR(symbol), PTR(value));
}

lisp_value *lisp_scope_lookup(lisp_runtime *rt, lisp_scope *scope,
                              lisp_symbol *symbol)
{
  smb_status status = SMB_SUCCESS;
  lisp_value *v = ht_get(&scope->scope, PTR(symbol), &status).data_ptr;
  if (status == SMB_NOT_FOUND_ERROR) {
    if (scope->up) {
      return lisp_scope_lookup(rt, scope->up, symbol);
    } else {
      return (lisp_value*)lisp_error_new(rt, "symbol not found in scope");
    }
  } else {
    return v;
  }
}

void lisp_scope_add_builtin(lisp_runtime *rt, lisp_scope *scope, char *name,
                            lisp_builtin_func call)
{
  lisp_symbol *symbol = lisp_symbol_new(rt, name);
  lisp_builtin *builtin = lisp_builtin_new(rt, name, call);
  lisp_scope_bind(scope, symbol, (lisp_value*)builtin);
}

void lisp_scope_replace_or_insert(lisp_scope *scope, lisp_symbol *key, lisp_value *value)
{
  lisp_scope *s = scope;

  // First go up the chain checking for the name.
  while (s) {
    if (ht_contains(&s->scope, PTR(key))) {
      // If we find it, replace it.
      ht_insert(&s->scope, PTR(key), PTR(value));
      return;
    }
    s = s->up;
  }

  // If we never find it, insert it in the "lowest" scope.
  ht_insert(&scope->scope, PTR(key), PTR(value));
}

lisp_symbol *lisp_symbol_new(lisp_runtime *rt, char *sym)
{
  lisp_symbol *err = (lisp_symbol*)lisp_new(rt, type_symbol);
  int len = strlen(sym);
  err->sym = malloc(len + 1);
  strncpy(err->sym, sym, len);
  err->sym[len] = '\0';
  return err;
}

lisp_error *lisp_error_new(lisp_runtime *rt, char *message)
{
  lisp_error *err = (lisp_error*)lisp_new(rt, type_error);
  int len = strlen(message);
  err->message = malloc(len + 1);
  strncpy(err->message, message, len);
  err->message[len] = '\0';
  return err;
}

lisp_builtin *lisp_builtin_new(lisp_runtime *rt, char *name,
                               lisp_builtin_func call)
{
  lisp_builtin *builtin = (lisp_builtin*)lisp_new(rt, type_builtin);
  builtin->call = call;
  builtin->name = name;
  return builtin;
}

lisp_value *lisp_nil_new(lisp_runtime *rt)
{
  if (rt->nil == NULL) {
    rt->nil = lisp_new(rt, type_list);
  }
  return rt->nil;
}

lisp_value *lisp_eval_list(lisp_runtime *rt, lisp_scope *scope, lisp_value *l)
{
  if (lisp_nil_p(l)) {
    return l;
  }
  lisp_list *list = (lisp_list*) l;
  lisp_list *result = (lisp_list*)lisp_new(rt, type_list);
  result->left = lisp_eval(rt, scope, list->left);
  result->right = lisp_eval_list(rt, scope, list->right);
  return (lisp_value*) result;
}

int lisp_list_length(lisp_list *list)
{
  int length = 0;
  while (list->type == type_list && !lisp_nil_p((lisp_value*)list)) {
    length++;
    list = (lisp_list*)list->right;
  }
  return length;
}

lisp_value *lisp_quote(lisp_runtime *rt, lisp_value *value) {
  lisp_list *l = (lisp_list*)lisp_new(rt, type_list);
  lisp_symbol *q = lisp_symbol_new(rt, "quote");
  l->left = (lisp_value*)q;
  lisp_list *s = (lisp_list*) lisp_new(rt, type_list);
  s->right = lisp_nil_new(rt);
  l->right = (lisp_value*)s;
  s->left = value;
  return (lisp_value*)l;
}

static lisp_type *lisp_get_type(char c)
{
  switch (c) {
  case 'd':
    return type_integer;
  case 'l':
    return type_list;
  case 's':
    return type_symbol;
  case 'S':
    return type_string;
  case 'o':
    return type_scope;
  case 'e':
    return type_error;
  case 'b':
    return type_builtin;
  case 't':
    return type_type;
  }
  return NULL;
}

bool lisp_get_args(lisp_list *list, char *format, ...)
{
  va_list va;
  va_start(va, format);
  lisp_value **v;
  while (!lisp_nil_p((lisp_value*)list) && *format != '\0') {
    lisp_type *type = lisp_get_type(*format);
    if (type != NULL && type != list->left->type) {
      return false;
    }
    v = va_arg(va, lisp_value**);
    *v = list->left;
    list = (lisp_list*)list->right;
    format += 1;
  }
  if (strlen(format) != 0 || !lisp_nil_p((lisp_value*)list)) {
    return false;
  }
  return true;
}

static lisp_value *lisp_builtin_eval(lisp_runtime *rt, lisp_scope *scope,
                                     lisp_value *arguments)
{
  lisp_list *evald = (lisp_list*)lisp_eval_list(rt, scope, arguments);
  lisp_value *result = lisp_eval(rt, scope, evald->left);
  return result;
}

static lisp_value *lisp_builtin_car(lisp_runtime *rt, lisp_scope *scope,
                                    lisp_value *a)
{
  lisp_list *firstarg;
  lisp_list *arglist = (lisp_list*) lisp_eval_list(rt, scope, a);
  if (!lisp_get_args(arglist, "l", &firstarg)) {
    return (lisp_value*)lisp_error_new(rt, "wrong arguments to car");
  }
  if (lisp_list_length(firstarg) == 0) {
    return (lisp_value*)lisp_error_new(rt, "expected at least one item");
  }
  return firstarg->left;
}

static lisp_value *lisp_builtin_cdr(lisp_runtime *rt, lisp_scope *scope,
                                    lisp_value *a)
{
  lisp_list *firstarg;
  lisp_list *arglist = (lisp_list*) lisp_eval_list(rt, scope, a);
  if (!lisp_get_args(arglist, "l", &firstarg)) {
    return (lisp_value*) lisp_error_new(rt, "wrong arguments to cdr");
  }
  // save rv because firstarg may be deleted after decref
  return firstarg->right;
}

static lisp_value *lisp_builtin_quote(lisp_runtime *rt, lisp_scope *scope,
                                      lisp_value *a)
{
  (void)scope;
  lisp_value *firstarg;
  lisp_list *arglist = (lisp_list*) a;
  if (!lisp_get_args(arglist, "*", &firstarg)) {
    return (lisp_value*) lisp_error_new(rt, "wrong arguments to quote");
  }
  return arglist->left;
}

static lisp_value *lisp_builtin_cons(lisp_runtime *rt, lisp_scope *scope,
                                     lisp_value *a)
{
  lisp_value *a1;
  lisp_value *l;
  lisp_list *arglist = (lisp_list*) lisp_eval_list(rt, scope, a);
  if (!lisp_get_args(arglist, "**", &a1, &l)) {
    return (lisp_value*) lisp_error_new(rt, "wrong arguments to cons");
  }
  lisp_list *new = (lisp_list*)lisp_new(rt, type_list);
  new->left = a1;
  new->right = (lisp_value*)l;
  return (lisp_value*)new;
}

static lisp_value *lisp_builtin_lambda(lisp_runtime *rt, lisp_scope *scope,
                                       lisp_value *a)
{
  lisp_list *argnames;
  lisp_value *code;
  lisp_list *our_args = (lisp_list*)a;
  (void)scope;

  if (!lisp_get_args(our_args, "l*", &argnames, &code)) {
    return (lisp_value*) lisp_error_new(rt, "expected argument list and code");
  }

  lisp_list *it = argnames;
  while (!lisp_nil_p((lisp_value*)it)) {
    if (it->left->type != type_symbol) {
      return (lisp_value*) lisp_error_new(rt, "argument names must be symbols");
    }
    it = (lisp_list*) it->right;
  }

  lisp_lambda *lambda = (lisp_lambda*)lisp_new(rt, type_lambda);
  lambda->args = argnames;
  lambda->code = code;
  lambda->closure = scope;
  return (lisp_value*) lambda;
}

static lisp_value *lisp_builtin_define(lisp_runtime *rt, lisp_scope *scope,
                                       lisp_value *a)
{
  lisp_symbol *s;
  lisp_value *expr;

  if (!lisp_get_args((lisp_list*)a, "s*", &s, &expr)) {
    return (lisp_value*) lisp_error_new(rt, "expected name and expression");
  }

  lisp_value *evald = lisp_eval(rt, scope, expr);
  lisp_scope_replace_or_insert(scope, s, evald);
  //lisp_scope_bind(scope, s, evald);
  return evald;
}

static lisp_value *lisp_builtin_plus(lisp_runtime *rt, lisp_scope *scope,
                                     lisp_value *a)
{
  lisp_integer *i;
  lisp_list *args = (lisp_list*)lisp_eval_list(rt, scope, a);
  int sum = 0;

  while (!lisp_nil_p((lisp_value*)args)) {
    if (args->left->type != type_integer) {
      return (lisp_value*) lisp_error_new(rt, "expect integers for addition");
    }
    i = (lisp_integer*) args->left;
    sum += i->x;
    args = (lisp_list*)args->right;
  }

  i = (lisp_integer*)lisp_new(rt, type_integer);
  i->x = sum;
  return (lisp_value*)i;
}

static lisp_value *lisp_builtin_minus(lisp_runtime *rt, lisp_scope *scope,
                                      lisp_value *a)
{
  lisp_integer *i;
  lisp_list *args = (lisp_list*)lisp_eval_list(rt, scope, a);
  int val = 0;
  int len = lisp_list_length(args);

  if (len < 1) {
    return (lisp_value*) lisp_error_new(rt, "expected at least one arg");
  } else if (len == 1) {
    i = (lisp_integer*) args->left;
    val = - i->x;
  } else {
    i = (lisp_integer*) args->left;
    val = i->x;
    args = (lisp_list*)args->right;
    while (!lisp_nil_p((lisp_value*)args)) {
      if (args->left->type != type_integer) {
        return (lisp_value*)lisp_error_new(rt, "expected integer");
      }
      i = (lisp_integer*) args->left;
      val -= i->x;
      args = (lisp_list*) args->right;
    }
  }

  i = (lisp_integer*)lisp_new(rt, type_integer);
  i->x = val;
  return (lisp_value*)i;
}

static lisp_value *lisp_builtin_multiply(lisp_runtime *rt, lisp_scope *scope,
                                         lisp_value *a)
{
  lisp_integer *i;
  lisp_list *args = (lisp_list*) lisp_eval_list(rt, scope, a);
  int product = 1;

  while (!lisp_nil_p((lisp_value*)args)) {
    if (args->left->type != type_integer) {
      return (lisp_value*) lisp_error_new(rt, "expect integers for multiplication");
    }
    i = (lisp_integer*) args->left;
    product *= i->x;
    args = (lisp_list*)args->right;
  }

  i = (lisp_integer*)lisp_new(rt, type_integer);
  i->x = product;
  return (lisp_value*)i;
}

static lisp_value *lisp_builtin_divide(lisp_runtime *rt, lisp_scope *scope,
                                       lisp_value *a)
{
  lisp_integer *i;
  lisp_list *args = (lisp_list*)lisp_eval_list(rt, scope, a);
  int val = 0;
  int len = lisp_list_length(args);

  if (len < 1) {
    return (lisp_value*) lisp_error_new(rt, "expected at least one arg");
  }
  i = (lisp_integer*) args->left;
  val = i->x;
  args = (lisp_list*)args->right;
  while (!lisp_nil_p((lisp_value*)args)) {
    if (args->left->type != type_integer) {
      return (lisp_value*)lisp_error_new(rt, "expected integer");
    }
    i = (lisp_integer*) args->left;
    if (i->x == 0) {
      return (lisp_value*) lisp_error_new(rt, "divide by zero");
    }
    val /= i->x;
    args = (lisp_list*) args->right;
  }

  i = (lisp_integer*)lisp_new(rt, type_integer);
  i->x = val;
  return (lisp_value*)i;
}

static lisp_value *lisp_builtin_cmp_util(lisp_runtime *rt, lisp_scope *scope,
                                         lisp_value *a)
{
  lisp_integer *first, *second;
  lisp_list *args = (lisp_list*) lisp_eval_list(rt, scope, a);

  if (!lisp_get_args((lisp_list*)args, "dd", &first, &second)) {
    return (lisp_value*) lisp_error_new(rt, "expected two integers");
  }

  lisp_integer *result = (lisp_integer*)lisp_new(rt, type_integer);
  result->x = first->x - second->x;
  return (lisp_value*)result;
}

static lisp_value *lisp_builtin_eq(lisp_runtime *rt, lisp_scope *scope,
                                   lisp_value *a)
{
  lisp_integer *v = (lisp_integer*)lisp_builtin_cmp_util(rt, scope, a);
  if (v->type == type_integer) {
    v->x = (v->x == 0);
  }
  return (lisp_value*)v;
}

static lisp_value *lisp_builtin_gt(lisp_runtime *rt, lisp_scope *scope,
                                   lisp_value *a)
{
  lisp_integer *v = (lisp_integer*)lisp_builtin_cmp_util(rt, scope, a);
  if (v->type == type_integer) {
    v->x = (v->x > 0);
  }
  return (lisp_value*)v;
}

static lisp_value *lisp_builtin_ge(lisp_runtime *rt, lisp_scope *scope,
                                   lisp_value *a)
{
  lisp_integer *v = (lisp_integer*)lisp_builtin_cmp_util(rt, scope, a);
  if (v->type == type_integer) {
    v->x = (v->x >= 0);
  }
  return (lisp_value*)v;
}

static lisp_value *lisp_builtin_lt(lisp_runtime *rt, lisp_scope *scope,
                                   lisp_value *a)
{
  lisp_integer *v = (lisp_integer*)lisp_builtin_cmp_util(rt, scope, a);
  if (v->type == type_integer) {
    v->x = (v->x < 0);
  }
  return (lisp_value*)v;
}

static lisp_value *lisp_builtin_le(lisp_runtime *rt, lisp_scope *scope,
                                   lisp_value *a)
{
  lisp_integer *v = (lisp_integer*)lisp_builtin_cmp_util(rt, scope, a);
  if (v->type == type_integer) {
    v->x = (v->x <= 0);
  }
  return (lisp_value*)v;
}

static lisp_value *lisp_builtin_if(lisp_runtime *rt, lisp_scope *scope,
                                   lisp_value *a)
{
  lisp_value *condition, *body_true, *body_false;

  if (!lisp_get_args((lisp_list*)a, "***", &condition, &body_true, &body_false)) {
    return (lisp_value*) lisp_error_new(rt, "expected condition and two bodies");
  }

  condition = lisp_eval(rt, scope, condition);
  if (condition->type == type_integer && ((lisp_integer*)condition)->x) {
    return lisp_eval(rt, scope, body_true);
  } else {
    return lisp_eval(rt, scope, body_false);
  }
}

static lisp_value *lisp_builtin_null_p(lisp_runtime *rt, lisp_scope *scope,
                                       lisp_value *a)
{
  lisp_value *v;
  lisp_list *args = (lisp_list*) lisp_eval_list(rt, scope, a);

  if (!lisp_get_args(args, "*", &v)) {
    return (lisp_value*) lisp_error_new(rt, "expected one argument");
  }

  lisp_integer *result = (lisp_integer*) lisp_new(rt, type_integer);
  result->x = (int) lisp_nil_p(v);
  return (lisp_value*)result;
}

static lisp_list *get_quoted_left_items(lisp_runtime *rt, lisp_list *list_of_lists)
{
  lisp_list *left_items = NULL, *rv;
  while (!lisp_nil_p((lisp_value*)list_of_lists)) {
    // Create or advance left_items to the next list.
    if (left_items == NULL) {
      left_items = (lisp_list*) lisp_new(rt, type_list);
      rv = left_items;
    } else {
      left_items->right = lisp_new(rt, type_list);
      left_items = (lisp_list*) left_items->right;
    }
    // Check the next node in the list to make sure it's actually a list.
    if (lisp_nil_p(list_of_lists->left)) {
      return NULL;
    }
    // Get the next node in the list and get the argument.
    lisp_list *l = (lisp_list*) list_of_lists->left;
    left_items->left = lisp_quote(rt, l->left);
    list_of_lists = (lisp_list*) list_of_lists->right;
  }
  left_items->right = lisp_nil_new(rt);
  return rv;
}

static lisp_list *advance_lists(lisp_runtime *rt, lisp_list *list_of_lists)
{
  lisp_list *right_items = NULL, *rv;
  while (!lisp_nil_p((lisp_value*)list_of_lists)) {
    // Create or advance left_items to the next list.
    if (right_items == NULL) {
      right_items = (lisp_list*) lisp_new(rt, type_list);
      rv = right_items;
    } else {
      right_items->right = lisp_new(rt, type_list);
      right_items = (lisp_list*) right_items->right;
    }
    // Check the next node in the list to make sure it's actually a list.
    if (list_of_lists->left->type != type_list) {
      return NULL;
    }
    // Get the next node in the list and get the argument.
    lisp_list *l = (lisp_list*) list_of_lists->left;
    right_items->left = l->right;
    list_of_lists = (lisp_list*) list_of_lists->right;
  }
  right_items->right = lisp_nil_new(rt);
  return rv;
}

static lisp_value *lisp_builtin_map(lisp_runtime *rt, lisp_scope *scope,
                                    lisp_value *a)
{
  lisp_value *f;
  lisp_list *ret = NULL, *args, *rv;
  lisp_list *map_args = (lisp_list *) lisp_eval_list(rt, scope, a);

  // Get the function from the first argument in the list.
  f = map_args->left;
  if (map_args->right->type != type_list) {
    return (lisp_value*) lisp_error_new(rt, "need at least two arguments");
  }
  map_args = (lisp_list*) map_args->right;
  while ((args = get_quoted_left_items(rt, map_args)) != NULL) {
    if (ret == NULL) {
      ret = (lisp_list*) lisp_new(rt, type_list);
      rv = ret;
    } else {
      ret->right = lisp_new(rt, type_list);
      ret = (lisp_list*) ret->right;
    }
    ret->left = lisp_call(rt, scope, f, (lisp_value*)args);
    map_args = advance_lists(rt, map_args);
  }
  ret->right = lisp_nil_new(rt);
  return (lisp_value*) rv;
}

static lisp_value *lisp_builtin_reduce(lisp_runtime *rt, lisp_scope *scope, lisp_value *a)
{
  lisp_list *args = (lisp_list*) lisp_eval_list(rt, scope, a);
  int length = lisp_list_length(args);
  lisp_value *callable, *initializer;
  lisp_list *list;

  if (length == 2) {
    if (!lisp_get_args(args, "*l", &callable, &list)) {
      return (lisp_value*) lisp_error_new(rt, "reduce: callable and list required");
    }
    if (lisp_list_length(list) < 2) {
      return (lisp_value*) lisp_error_new(rt, "reduce: list must have at least 2 entries");
    }
    initializer = list->left;
    list = (lisp_list*)list->right;
 } else if (length == 3) {
    if (!lisp_get_args(args, "**l", &callable, &initializer, &list)) {
      return (lisp_value*) lisp_error_new(rt, "reduce: callable, initializer, and list required");
    }
    if (lisp_list_length(list) < 1) {
      return (lisp_value*) lisp_error_new(rt, "reduce: list must have at least 1 entry");
    }
  } else {
    return (lisp_value*) lisp_error_new(rt, "reduce: 2 or 3 arguments required");
  }

  while (!lisp_nil_p((lisp_value*)list)) {
    initializer = lisp_call(rt, scope, callable,
                            (lisp_value*) lisp_new_pair_list(rt, initializer, list->left));
    list = (lisp_list*) list->right;
  }
  return initializer;
}

void lisp_scope_populate_builtins(lisp_runtime *rt, lisp_scope *scope)
{
  lisp_scope_add_builtin(rt, scope, "eval", lisp_builtin_eval);
  lisp_scope_add_builtin(rt, scope, "car", lisp_builtin_car);
  lisp_scope_add_builtin(rt, scope, "cdr", lisp_builtin_cdr);
  lisp_scope_add_builtin(rt, scope, "quote", lisp_builtin_quote);
  lisp_scope_add_builtin(rt, scope, "cons", lisp_builtin_cons);
  lisp_scope_add_builtin(rt, scope, "lambda", lisp_builtin_lambda);
  lisp_scope_add_builtin(rt, scope, "define", lisp_builtin_define);
  lisp_scope_add_builtin(rt, scope, "+", lisp_builtin_plus);
  lisp_scope_add_builtin(rt, scope, "-", lisp_builtin_minus);
  lisp_scope_add_builtin(rt, scope, "*", lisp_builtin_multiply);
  lisp_scope_add_builtin(rt, scope, "/", lisp_builtin_divide);
  lisp_scope_add_builtin(rt, scope, "==", lisp_builtin_eq);
  lisp_scope_add_builtin(rt, scope, "=", lisp_builtin_eq);
  lisp_scope_add_builtin(rt, scope, ">", lisp_builtin_gt);
  lisp_scope_add_builtin(rt, scope, ">=", lisp_builtin_ge);
  lisp_scope_add_builtin(rt, scope, "<", lisp_builtin_lt);
  lisp_scope_add_builtin(rt, scope, "<=", lisp_builtin_le);
  lisp_scope_add_builtin(rt, scope, "if", lisp_builtin_if);
  lisp_scope_add_builtin(rt, scope, "null?", lisp_builtin_null_p);
  lisp_scope_add_builtin(rt, scope, "map", lisp_builtin_map);
  lisp_scope_add_builtin(rt, scope, "reduce", lisp_builtin_reduce);
}
