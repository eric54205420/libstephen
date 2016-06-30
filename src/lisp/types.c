#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libstephen/lisp.h"
#include "libstephen/ht.h"

static lisp_value *eval_error(lisp_scope *s, lisp_value *v)
{
  (void)s;
  return (lisp_value*) lisp_error_new("cannot evaluate this object");
}

static lisp_value *eval_same(lisp_scope *s, lisp_value *v)
{
  (void)s;
  lisp_incref(v);
  return v;
}

static lisp_value *call_error(lisp_scope *s, lisp_value *c, lisp_value *v)
{
  (void)s;
  (void)c;
  (void)v;
  return (lisp_value*) lisp_error_new("not callable!");
}

static void nop_free(void *v)
{
  (void)v;
}

// type

static void type_print(FILE *f, lisp_value *v);
static lisp_value *type_new(void);

static lisp_type type_type_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="type",
  .print=type_print,
  .new=type_new,
  .eval=eval_error,
  .free=free,
  .call=call_error,
};
lisp_type *type_type = &type_type_obj;

static void type_print(FILE *f, lisp_value *v)
{
  lisp_type *value = (lisp_type*) v;
  fprintf(f, "%s", value->name);
}

static lisp_value *type_new(void)
{
  lisp_type *type = malloc(sizeof(lisp_type));
  type->refcount = 1;
  type->type = (lisp_value*)type_type;
  return (lisp_value*)type;
}

// scope

static void scope_print(FILE *f, lisp_value*v);
static lisp_value *scope_new(void);
static void scope_free(lisp_value *v);

static lisp_type type_scope_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="scope",
  .print=scope_print,
  .new=scope_new,
  .eval=eval_error,
  .free=free,
  .call=call_error,
};
lisp_type *type_scope = &type_scope_obj;

static unsigned int symbol_hash(DATA symbol)
{
  lisp_symbol *sym = symbol.data_ptr;
  return ht_string_hash(PTR(sym->sym));
}

static int symbol_compare(DATA d1, DATA d2)
{
  lisp_symbol *sym1 = d1.data_ptr;
  lisp_symbol *sym2 = d2.data_ptr;
  return data_compare_string(PTR(sym1->sym), PTR(sym2->sym));;
}

static lisp_value *scope_new(void)
{
  lisp_scope *scope = malloc(sizeof(lisp_scope));
  scope->type = (lisp_value*)type_scope;
  scope->refcount = 1;
  ht_init(&scope->scope, symbol_hash, symbol_compare);
  return (lisp_value*)scope;
}

static void scope_free(lisp_value *v)
{
  lisp_scope *scope = (lisp_scope*) v;
  smb_iter it = ht_get_iter(&scope->scope);
  while (it.has_next(&it)) {
    smb_status status = SMB_SUCCESS;
    lisp_value *key = it.next(&it, &status).data_ptr;
    assert(status == SMB_SUCCESS);
    lisp_value *value = ht_get(&scope->scope, PTR(key), &status).data_ptr;
    assert(status == SMB_SUCCESS);
    lisp_decref(key);
    lisp_decref(value);
  }
  ht_destroy(&scope->scope);
  free(scope);
}

static void scope_print(FILE *f, lisp_value *v)
{
  lisp_scope *scope = (lisp_scope*) v;
  smb_iter it = ht_get_iter(&scope->scope);
  fprintf(f, "(scope:");
  while (it.has_next(&it)) {
    smb_status status = SMB_SUCCESS;
    lisp_value *key = it.next(&it, &status).data_ptr;
    assert(status == SMB_SUCCESS);
    lisp_value *value = ht_get(&scope->scope, PTR(key), &status).data_ptr;
    assert(status == SMB_SUCCESS);
    fprintf(f, " ");
    lisp_print(f, key);
    fprintf(f, "=>");
    lisp_print(f, value);
  }
  fprintf(f, ")");
}

void lisp_scope_bind(lisp_scope *scope, lisp_symbol *symbol, lisp_value *value)
{
  ht_insert(&scope->scope, PTR(symbol), PTR(value));
}

lisp_value *lisp_scope_lookup(lisp_scope *scope, lisp_symbol *symbol)
{
  smb_status status = SMB_SUCCESS;
  lisp_value *v = ht_get(&scope->scope, PTR(symbol), &status).data_ptr;
  if (status == SMB_NOT_FOUND_ERROR) {
    if (scope->up) {
      return lisp_scope_lookup(scope->up, symbol);
    } else {
      return (lisp_value*)lisp_error_new("symbol not found in scope");
    }
  } else {
    return v;
  }
}

// list

static void list_print(FILE *f, lisp_value *v);
static lisp_value *list_new(void);
static lisp_value *list_eval(lisp_scope *scope, lisp_value *list);

static lisp_type type_list_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="list",
  .print=list_print,
  .new=list_new,
  .eval=list_eval,
  .free=free,
  .call=call_error,
};
lisp_type *type_list = &type_list_obj;

static lisp_value *list_eval(lisp_scope *scope, lisp_value *v)
{
  lisp_list *list = (lisp_list*) v;
  if (list->right->type != (lisp_value*) type_list) {
    return (lisp_value*) lisp_error_new("bad function call syntax");
  }
  lisp_value *callable = lisp_eval(scope, list->left);
  return lisp_call(scope, callable,(lisp_list*) list->right);
}

static void list_print_internal(FILE *f, lisp_list *list)
{
  lisp_print(f, list->left);
  if (list->right == lisp_nilv) {
    return;
  } else if (list->right->type != (lisp_value*)type_list) {
    fprintf(f, " . ");
    lisp_print(f, list->right);
    return;
  } else {
    fprintf(f, " ");
    list_print_internal(f, (lisp_list*)list->right);
  }
}

static void list_print(FILE *f, lisp_value *v)
{
  fprintf(f, "(");
  list_print_internal(f, (lisp_list*)v);
  fprintf(f, ")");
}

static lisp_value *list_new(void)
{
  lisp_list *list = malloc(sizeof(lisp_list));
  list->refcount = 1;
  list->type = (lisp_value*) type_list;
  list->left = NULL;
  list->right = NULL;
  return (lisp_value*) list;
}

// symbol

static void symbol_print(FILE *f, lisp_value *v);
static lisp_value *symbol_new(void);
static lisp_value *symbol_eval(lisp_scope *scope, lisp_value *value);
static void symbol_free(void *v);

static lisp_type type_symbol_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="symbol",
  .print=symbol_print,
  .new=symbol_new,
  .eval=symbol_eval,
  .free=symbol_free,
  .call=call_error,
};
lisp_type *type_symbol = &type_symbol_obj;

static void symbol_print(FILE *f, lisp_value *v)
{
  lisp_symbol *symbol = (lisp_symbol*) v;
  fprintf(f, "%s", symbol->sym);
}

static lisp_value *symbol_new(void)
{
  lisp_symbol *symbol = malloc(sizeof(lisp_symbol*));
  symbol->refcount = 1;
  symbol->type = (lisp_value*) type_symbol;
  symbol->sym = NULL;
  return (lisp_value*)symbol;
}

static lisp_value *symbol_eval(lisp_scope *scope, lisp_value *value)
{
  lisp_symbol *symbol = (lisp_symbol*) value;
  return lisp_scope_lookup(scope, symbol);
}

static void symbol_free(void *v)
{
  lisp_symbol *symbol = (lisp_symbol*) v;
  free(symbol->sym);
  free(symbol);
}

lisp_symbol *lisp_symbol_new(char *sym)
{
  lisp_symbol *err = (lisp_symbol*)type_symbol->new();
  int len = strlen(sym);
  err->sym = malloc(len + 1);
  strncpy(err->sym, sym, len);
  err->sym[len] = '\0';
  return err;
}

// error

static void error_print(FILE *f, lisp_value *v);
static lisp_value *error_new(void);
static void error_free(void *v);

static lisp_type type_error_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="error",
  .print=error_print,
  .new=error_new,
  .eval=eval_same,
  .free=error_free,
  .call=call_error,
};
lisp_type *type_error = &type_error_obj;

static void error_print(FILE *f, lisp_value *v)
{
  lisp_error *error = (lisp_error*) v;
  fprintf(f, "'%s'", error->message);
}

static lisp_value *error_new(void)
{
  lisp_error *error = malloc(sizeof(lisp_error*));
  error->refcount = 1;
  error->type = (lisp_value*) type_error;
  error->message = NULL;
  return (lisp_value*)error;
}

static void error_free(void *v)
{
  lisp_error *error = (lisp_error*) v;
  free(error->message);
  free(error);
}

lisp_error *lisp_error_new(char *message)
{
  lisp_error *err = (lisp_error*)type_error->new();
  int len = strlen(message);
  err->message = malloc(len + 1);
  strncpy(err->message, message, len);
  err->message[len] = '\0';
  return err;
}

// integer

static void integer_print(FILE *f, lisp_value *v);
static lisp_value *integer_new(void);

static lisp_type type_integer_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="integer",
  .print=integer_print,
  .new=integer_new,
  .eval=eval_same,
  .free=free,
  .call=call_error,
};
lisp_type *type_integer = &type_integer_obj;

static void integer_print(FILE *f, lisp_value *v)
{
  lisp_integer *integer = (lisp_integer*) v;
  fprintf(f, "%d", integer->x);
}

static lisp_value *integer_new(void)
{
  lisp_integer *integer = malloc(sizeof(lisp_integer*));
  integer->refcount = 1;
  integer->type = (lisp_value*)type_integer;
  integer->x = 0;
  return (lisp_value*)integer;
}

// nil

static void nil_print(FILE *f, lisp_value *v);
static lisp_value *nil_new();

static lisp_type type_nil_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="nil",
  .print=nil_print,
  .new=nil_new,
  .eval=eval_error,
  .free=nop_free,
  .call=call_error,
};
lisp_type *type_nil = &type_nil_obj;

static lisp_nil lisp_nil_obj = {
  .type=(lisp_value*)&type_nil_obj,
  .refcount=1,
};
lisp_value *lisp_nilv = (lisp_value*) &lisp_nil_obj;

static void nil_print(FILE *f, lisp_value *v)
{
  (void)v;
  fprintf(f, "'()");
}

static lisp_value *nil_new(void)
{
  return lisp_nilv;
}

// builtin

static void builtin_print(FILE *f, lisp_value *v);
static lisp_value *builtin_new();
static lisp_value *builtin_call(lisp_scope *scope, lisp_value *c,
                                lisp_value *arguments);

static lisp_type type_builtin_obj = {
  .type=(lisp_value*)&type_type_obj,
  .refcount=1,
  .name="builtin",
  .print=builtin_print,
  .new=builtin_new,
  .eval=eval_error,
  .free=nop_free,
  .call=builtin_call,
};
lisp_type *type_builtin = &type_builtin_obj;

static void builtin_print(FILE *f, lisp_value *v)
{
  lisp_builtin *builtin = (lisp_builtin*) v;
  fprintf(f, "<builtin function %s>", builtin->name);
}

static lisp_value *builtin_new()
{
  lisp_builtin *builtin = malloc(sizeof(lisp_builtin));
  builtin->refcount = 1;
  builtin->type = (lisp_value*) type_builtin;
  builtin->call = NULL;
  builtin->name = NULL;
  return (lisp_value*) builtin;
}

static lisp_value *builtin_call(lisp_scope *scope, lisp_value *c,
                                lisp_value *arguments)
{
  lisp_builtin *builtin = (lisp_builtin*) c;
  return builtin->call(scope, arguments);
}

lisp_builtin *lisp_builtin_new(char *name, lisp_value *(*call)(lisp_scope *, lisp_value *))
{
  lisp_builtin *builtin = (lisp_builtin*)type_builtin->new();
  builtin->call = call;
  builtin->name = name;
  return builtin;
}

lisp_value *lisp_eval_list(lisp_scope *scope, lisp_value *l)
{
  if (l == lisp_nilv) {
    lisp_incref(l);
    return l;
  }
  lisp_list *list = (lisp_list*) l;
  lisp_list *result = (lisp_list*) type_list->new();
  result->left = lisp_eval(scope, list->left);
  result->right = lisp_eval_list(scope, list->right);
  return (lisp_value*) result;
}

static lisp_value *lisp_builtin_eval(lisp_scope *scope, lisp_value *arguments)
{
  lisp_list *evald = (lisp_list*)lisp_eval_list(scope, arguments);
  return lisp_eval(scope, evald->left);
}

static lisp_value *lisp_builtin_car(lisp_scope *scope, lisp_value *a)
{
  lisp_list *arglist = (lisp_list*) lisp_eval_list(scope, a);
  lisp_list *firstarg = (lisp_list*) arglist->left;
  return firstarg->left;
}

static lisp_value *lisp_builtin_cdr(lisp_scope *scope, lisp_value *a)
{
  lisp_list *arglist = (lisp_list*) lisp_eval_list(scope, a);
  lisp_list *firstarg = (lisp_list*) arglist->left;
  return firstarg->right;
}

static lisp_value *lisp_builtin_quote(lisp_scope *scope, lisp_value *a)
{
  lisp_list *arglist = (lisp_list*) a;
  return arglist->left;
}

static void lisp_scope_add_builtin(lisp_scope *scope, char *name, lisp_value * (*call)(lisp_scope*,lisp_value*))
{
  lisp_symbol *symbol = lisp_symbol_new(name);
  lisp_builtin *builtin = lisp_builtin_new(name, call);
  lisp_scope_bind(scope, symbol, (lisp_value*)builtin);
}

void lisp_scope_populate_builtins(lisp_scope *scope)
{
  lisp_scope_add_builtin(scope, "eval", lisp_builtin_eval);
  lisp_scope_add_builtin(scope, "car", lisp_builtin_car);
  lisp_scope_add_builtin(scope, "cdr", lisp_builtin_cdr);
  lisp_scope_add_builtin(scope, "quote", lisp_builtin_quote);
}

void lisp_print(FILE *f, lisp_value *value)
{
  lisp_type *type = (lisp_type*) value->type;
  type->print(f, value);
}

void lisp_free(lisp_value *value)
{
  lisp_type *type = (lisp_type*) value->type;
  type->free(value);
}

lisp_value *lisp_eval(lisp_scope *scope, lisp_value *value)
{
  lisp_type *type = (lisp_type*) value->type;
  return type->eval(scope, value);
}

lisp_value *lisp_call(lisp_scope *scope, lisp_value *callable, lisp_value *args)
{
  lisp_type *type = (lisp_type*) callable->type;
  return type->call(scope, callable, args);
}

void lisp_incref(lisp_value *value)
{
  value->refcount++;
}

void lisp_decref(lisp_value *value)
{
  lisp_type *type = (lisp_type*) value->type;
  value->refcount--;
  if (value->refcount <= 0) {
    type->free(value);
  }
}
