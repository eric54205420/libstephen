/***************************************************************************//**

  @file         parse.c

  @author       Stephen Brennan

  @date         Created Wednesday, 10 February 2016

  @brief        Parser tests.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the Revised
                BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include "libstephen/ut.h"
#include "tests.h"

#include "libstephen/re.h"
#include "libstephen/re_internals.h"

/*
  Simple convenience functions for a parser.
  REDEFINED HERE SO THAT THEY DON'T POLLUTE THE GLOBAL NAMESPACE.
 */

static void expect(TSym s, Lexer *l)
{
  if (l->tok.sym == s) {
    nextsym(l);
    return;
  }
  fprintf(stderr, "error: expected %s, got %s\n", names[s], names[l->tok.sym]);
  exit(1);
}

static int test_TERM_CharSym_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->tok.sym, CharSym);
  TA_WCHAR_EQ(tree->children[0]->tok.c, L'a');

  free_tree(tree);
  return 0;
}

static int test_TERM_Minus_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"-";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->tok.sym, Minus);
  TA_WCHAR_EQ(tree->children[0]->tok.c, L'-');

  free_tree(tree);
  return 0;
}

static int test_TERM_Caret_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"^";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->tok.sym, Caret);
  TA_WCHAR_EQ(tree->children[0]->tok.c, L'^');

  free_tree(tree);
  return 0;
}

static int test_TERM_Dot_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L".";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->tok.sym, Dot);
  TA_WCHAR_EQ(tree->children[0]->tok.c, L'.');

  free_tree(tree);
  return 0;
}

static int test_TERM_Special_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"\\w";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->tok.sym, Special);
  TA_WCHAR_EQ(tree->children[0]->tok.c, L'w');

  free_tree(tree);
  return 0;
}

static int test_TERM_Subexpr_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"(a+)";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[1]->nt, REGEXnt);

  free_tree(tree);
  return 0;
}

static int test_TERM_Class_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"[abc]";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->production, 3);
  TA_INT_EQ(tree->children[1]->nt, CLASSnt);

  free_tree(tree);
  return 0;
}

static int test_TERM_NClass_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"[^abc]";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->production, 4);
  TA_INT_EQ(tree->children[1]->nt, CLASSnt);

  free_tree(tree);
  return 0;
}

static int test_EXPR_Term_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);

  free_tree(tree);
  return 0;
}

static int test_EXPR_Plus_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a+";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 2);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TEST_ASSERT(tree->children[1]->tok.sym == Plus);

  free_tree(tree);
  return 0;
}

static int test_EXPR_PlusQuestion_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a+?";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TEST_ASSERT(tree->children[1]->tok.sym == Plus);
  TEST_ASSERT(tree->children[2]->tok.sym == Question);

  free_tree(tree);
  return 0;
}

static int test_EXPR_Star_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a*";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 2);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TEST_ASSERT(tree->children[1]->tok.sym == Star);

  free_tree(tree);
  return 0;
}

static int test_EXPR_StarQuestion_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a*?";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TEST_ASSERT(tree->children[1]->tok.sym == Star);
  TEST_ASSERT(tree->children[2]->tok.sym == Question);

  free_tree(tree);
  return 0;
}

static int test_EXPR_Question_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a?";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 2);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TEST_ASSERT(tree->children[1]->tok.sym == Question);

  free_tree(tree);
  return 0;
}

static int test_EXPR_QuestionQuestion_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a??";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TEST_ASSERT(tree->children[1]->tok.sym == Question);
  TEST_ASSERT(tree->children[2]->tok.sym == Question);

  free_tree(tree);
  return 0;
}

static int test_SUB_Normal_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = SUB(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, SUBnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->nt, EXPRnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  free_tree(tree);
  return 0;
}

static int test_SUB_Concat_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"ab";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = SUB(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, SUBnt);
  TA_INT_EQ(tree->nchildren, 2);
  TA_INT_EQ(tree->children[0]->nt, EXPRnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TA_INT_EQ(tree->children[1]->nt, SUBnt);
  TA_INT_EQ(tree->children[1]->nchildren, 1);
  TA_INT_EQ(tree->children[1]->children[0]->nt, EXPRnt);

  free_tree(tree);
  return 0;
}

static int test_REGEX_Normal_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = REGEX(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, REGEXnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->nt, SUBnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);

  free_tree(tree);
  return 0;
}

static int test_REGEX_Alternate_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a|b";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = REGEX(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, REGEXnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[0]->nt, SUBnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TA_INT_EQ(tree->children[1]->tok.sym, Pipe);
  TA_INT_EQ(tree->children[2]->nt, REGEXnt);
  TA_INT_EQ(tree->children[2]->nchildren, 1);
  TA_INT_EQ(tree->children[2]->children[0]->nt, SUBnt);

  free_tree(tree);
  return 0;
}

static int test_CLASS_range_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a-b";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = CLASS(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, CLASSnt);
  TA_INT_EQ(tree->nchildren, 2);
  TA_INT_EQ(tree->children[0]->tok.sym, CharSym);
  TA_WCHAR_EQ(tree->children[0]->tok.c, L'a');
  TA_INT_EQ(tree->children[1]->tok.sym, CharSym);
  TA_WCHAR_EQ(tree->children[1]->tok.c, L'b');

  free_tree(tree);
  return 0;
}

static int test_CLASS_range_range_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a-b1-2";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = CLASS(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, CLASSnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[0]->tok.sym, CharSym);
  TA_WCHAR_EQ(tree->children[0]->tok.c, L'a');
  TA_INT_EQ(tree->children[1]->tok.sym, CharSym);
  TA_WCHAR_EQ(tree->children[1]->tok.c, L'b');
  TA_INT_EQ(tree->children[2]->nt, CLASSnt);
  TA_INT_EQ(tree->children[2]->nchildren, 2);
  TA_INT_EQ(tree->children[2]->children[0]->tok.sym, CharSym);
  TA_WCHAR_EQ(tree->children[2]->children[0]->tok.c, L'1');
  TA_INT_EQ(tree->children[2]->children[1]->tok.sym, CharSym);
  TA_WCHAR_EQ(tree->children[2]->children[1]->tok.c, L'2');

  free_tree(tree);
  return 0;
}

static int test_CLASS_single_wide(void)
{
  wchar_t *accept[] = {L".", L"+", L"*", L"?", L"(", L")", L"|"};
  for (size_t i = 0; i < nelem(accept); i++) {
    Lexer l;
    l.tok = (Token){.sym=0, .c=0};
    l.input.wstr = accept[i];
    l.input.str = NULL;
    l.index = 0;
    l.nbuf = 0;

    nextsym(&l);
    PTree *tree = CLASS(&l);
    expect(Eof, &l);

    TA_PTR_NE(tree, NULL);
    TA_INT_EQ(tree->nt, CLASSnt);
    TA_INT_EQ(tree->nchildren, 1);
    TA_INT_EQ(tree->children[0]->tok.sym, CharSym);
    TA_INT_EQ(tree->children[0]->tok.c, accept[i][0]);

    free_tree(tree);
  }
  return 0;
}

static int test_CLASS_single_hyphen_wide(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.wstr = L"a-";
  l.input.str = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = CLASS(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, CLASSnt);
  TA_INT_EQ(tree->nchildren, 2);
  TA_INT_EQ(tree->children[0]->tok.sym, CharSym);
  TA_WCHAR_EQ(tree->children[0]->tok.c, L'a');
  TA_INT_EQ(tree->children[1]->nt, CLASSnt);
  TA_INT_EQ(tree->children[1]->nchildren, 1);
  TA_INT_EQ(tree->children[1]->children[0]->tok.sym, Minus);

  free_tree(tree);
  return 0;
}

static int test_reparse_wide(void)
{
  PTree *tree = reparsew(L"a+|b*");

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, REGEXnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[0]->nt, SUBnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TA_INT_EQ(tree->children[0]->children[0]->nt, EXPRnt);
  TA_INT_EQ(tree->children[0]->children[0]->nchildren, 2);
  TA_INT_EQ(tree->children[0]->children[0]->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->children[0]->children[0]->nchildren, 1);
  TA_INT_EQ(tree->children[0]->children[0]->children[0]->children[0]->tok.sym, CharSym);
  TA_WCHAR_EQ(tree->children[0]->children[0]->children[0]->children[0]->tok.c, L'a');
  TA_INT_EQ(tree->children[0]->children[0]->children[1]->tok.sym, Plus);
  TA_INT_EQ(tree->children[1]->tok.sym, Pipe);
  TA_INT_EQ(tree->children[2]->nt, REGEXnt);
  TA_INT_EQ(tree->children[2]->nchildren, 1);
  TA_INT_EQ(tree->children[2]->children[0]->nt, SUBnt);
  TA_INT_EQ(tree->children[2]->children[0]->nchildren, 1);
  TA_INT_EQ(tree->children[2]->children[0]->children[0]->nt, EXPRnt);
  TA_INT_EQ(tree->children[2]->children[0]->children[0]->nchildren, 2);
  TA_INT_EQ(tree->children[2]->children[0]->children[0]->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[2]->children[0]->children[0]->children[0]->nchildren, 1);
  TA_INT_EQ(tree->children[2]->children[0]->children[0]->children[0]->children[0]->tok.sym, CharSym);
  TA_WCHAR_EQ(tree->children[2]->children[0]->children[0]->children[0]->children[0]->tok.c, L'b');
  TA_INT_EQ(tree->children[2]->children[0]->children[0]->children[1]->tok.sym, Star);

  free_tree(tree);
  return 0;
}

static int test_TERM_CharSym(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->tok.sym, CharSym);
  TA_CHAR_EQ(tree->children[0]->tok.c, 'a');

  free_tree(tree);
  return 0;
}

static int test_TERM_Minus(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "-";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->tok.sym, Minus);
  TA_CHAR_EQ(tree->children[0]->tok.c, '-');

  free_tree(tree);
  return 0;
}

static int test_TERM_Caret(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "^";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->tok.sym, Caret);
  TA_CHAR_EQ(tree->children[0]->tok.c, '^');

  free_tree(tree);
  return 0;
}

static int test_TERM_Dot(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = ".";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->tok.sym, Dot);
  TA_CHAR_EQ(tree->children[0]->tok.c, '.');

  free_tree(tree);
  return 0;
}

static int test_TERM_Special(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "\\w";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->tok.sym, Special);
  TA_CHAR_EQ(tree->children[0]->tok.c, 'w');

  free_tree(tree);
  return 0;
}

static int test_TERM_Subexpr(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "(a+)";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[1]->nt, REGEXnt);

  free_tree(tree);
  return 0;
}

static int test_TERM_Class(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "[abc]";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->production, 3);
  TA_INT_EQ(tree->children[1]->nt, CLASSnt);

  free_tree(tree);
  return 0;
}

static int test_TERM_NClass(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "[^abc]";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = TERM(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, TERMnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->production, 4);
  TA_INT_EQ(tree->children[1]->nt, CLASSnt);

  free_tree(tree);
  return 0;
}

static int test_EXPR_Term(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);

  free_tree(tree);
  return 0;
}

static int test_EXPR_Plus(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a+";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 2);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TEST_ASSERT(tree->children[1]->tok.sym == Plus);

  free_tree(tree);
  return 0;
}

static int test_EXPR_PlusQuestion(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a+?";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TEST_ASSERT(tree->children[1]->tok.sym == Plus);
  TEST_ASSERT(tree->children[2]->tok.sym == Question);

  free_tree(tree);
  return 0;
}

static int test_EXPR_Star(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a*";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 2);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TEST_ASSERT(tree->children[1]->tok.sym == Star);

  free_tree(tree);
  return 0;
}

static int test_EXPR_StarQuestion(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a*?";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TEST_ASSERT(tree->children[1]->tok.sym == Star);
  TEST_ASSERT(tree->children[2]->tok.sym == Question);

  free_tree(tree);
  return 0;
}

static int test_EXPR_Question(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a?";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 2);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TEST_ASSERT(tree->children[1]->tok.sym == Question);

  free_tree(tree);
  return 0;
}

static int test_EXPR_QuestionQuestion(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a??";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = EXPR(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, EXPRnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TEST_ASSERT(tree->children[1]->tok.sym == Question);
  TEST_ASSERT(tree->children[2]->tok.sym == Question);

  free_tree(tree);
  return 0;
}

static int test_SUB_Normal(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = SUB(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, SUBnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->nt, EXPRnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  free_tree(tree);
  return 0;
}

static int test_SUB_Concat(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "ab";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = SUB(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, SUBnt);
  TA_INT_EQ(tree->nchildren, 2);
  TA_INT_EQ(tree->children[0]->nt, EXPRnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TA_INT_EQ(tree->children[1]->nt, SUBnt);
  TA_INT_EQ(tree->children[1]->nchildren, 1);
  TA_INT_EQ(tree->children[1]->children[0]->nt, EXPRnt);

  free_tree(tree);
  return 0;
}

static int test_REGEX_Normal(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = REGEX(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, REGEXnt);
  TA_INT_EQ(tree->nchildren, 1);
  TA_INT_EQ(tree->children[0]->nt, SUBnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);

  free_tree(tree);
  return 0;
}

static int test_REGEX_Alternate(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a|b";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = REGEX(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, REGEXnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[0]->nt, SUBnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TA_INT_EQ(tree->children[1]->tok.sym, Pipe);
  TA_INT_EQ(tree->children[2]->nt, REGEXnt);
  TA_INT_EQ(tree->children[2]->nchildren, 1);
  TA_INT_EQ(tree->children[2]->children[0]->nt, SUBnt);

  free_tree(tree);
  return 0;
}

static int test_CLASS_range(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a-b";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = CLASS(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, CLASSnt);
  TA_INT_EQ(tree->nchildren, 2);
  TA_INT_EQ(tree->children[0]->tok.sym, CharSym);
  TA_CHAR_EQ(tree->children[0]->tok.c, 'a');
  TA_INT_EQ(tree->children[1]->tok.sym, CharSym);
  TA_CHAR_EQ(tree->children[1]->tok.c, 'b');

  free_tree(tree);
  return 0;
}

static int test_CLASS_range_range(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a-b1-2";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = CLASS(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, CLASSnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[0]->tok.sym, CharSym);
  TA_CHAR_EQ(tree->children[0]->tok.c, 'a');
  TA_INT_EQ(tree->children[1]->tok.sym, CharSym);
  TA_CHAR_EQ(tree->children[1]->tok.c, 'b');
  TA_INT_EQ(tree->children[2]->nt, CLASSnt);
  TA_INT_EQ(tree->children[2]->nchildren, 2);
  TA_INT_EQ(tree->children[2]->children[0]->tok.sym, CharSym);
  TA_CHAR_EQ(tree->children[2]->children[0]->tok.c, '1');
  TA_INT_EQ(tree->children[2]->children[1]->tok.sym, CharSym);
  TA_CHAR_EQ(tree->children[2]->children[1]->tok.c, '2');

  free_tree(tree);
  return 0;
}

static int test_CLASS_single(void)
{
  char *accept[] = {".", "+", "*", "?", "(", ")", "|"};
  for (size_t i = 0; i < nelem(accept); i++) {
    Lexer l;
    l.tok = (Token){.sym=0, .c=0};
    l.input.str = accept[i];
    l.input.wstr = NULL;
    l.index = 0;
    l.nbuf = 0;

    nextsym(&l);
    PTree *tree = CLASS(&l);
    expect(Eof, &l);

    TA_PTR_NE(tree, NULL);
    TA_INT_EQ(tree->nt, CLASSnt);
    TA_INT_EQ(tree->nchildren, 1);
    TA_INT_EQ(tree->children[0]->tok.sym, CharSym);
    TA_INT_EQ(tree->children[0]->tok.c, accept[i][0]);

    free_tree(tree);
  }
  return 0;
}

static int test_CLASS_single_hyphen(void)
{
  Lexer l;
  l.tok = (Token){.sym=0, .c=0};
  l.input.str = "a-";
  l.input.wstr = NULL;
  l.index = 0;
  l.nbuf = 0;

  nextsym(&l);
  PTree *tree = CLASS(&l);
  expect(Eof, &l);

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, CLASSnt);
  TA_INT_EQ(tree->nchildren, 2);
  TA_INT_EQ(tree->children[0]->tok.sym, CharSym);
  TA_CHAR_EQ(tree->children[0]->tok.c, 'a');
  TA_INT_EQ(tree->children[1]->nt, CLASSnt);
  TA_INT_EQ(tree->children[1]->nchildren, 1);
  TA_INT_EQ(tree->children[1]->children[0]->tok.sym, Minus);

  free_tree(tree);
  return 0;
}

static int test_reparse(void)
{
  PTree *tree = reparse("a+|b*");

  TA_PTR_NE(tree, NULL);
  TA_INT_EQ(tree->nt, REGEXnt);
  TA_INT_EQ(tree->nchildren, 3);
  TA_INT_EQ(tree->children[0]->nt, SUBnt);
  TA_INT_EQ(tree->children[0]->nchildren, 1);
  TA_INT_EQ(tree->children[0]->children[0]->nt, EXPRnt);
  TA_INT_EQ(tree->children[0]->children[0]->nchildren, 2);
  TA_INT_EQ(tree->children[0]->children[0]->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[0]->children[0]->children[0]->nchildren, 1);
  TA_INT_EQ(tree->children[0]->children[0]->children[0]->children[0]->tok.sym, CharSym);
  TA_CHAR_EQ(tree->children[0]->children[0]->children[0]->children[0]->tok.c, 'a');
  TA_INT_EQ(tree->children[0]->children[0]->children[1]->tok.sym, Plus);
  TA_INT_EQ(tree->children[1]->tok.sym, Pipe);
  TA_INT_EQ(tree->children[2]->nt, REGEXnt);
  TA_INT_EQ(tree->children[2]->nchildren, 1);
  TA_INT_EQ(tree->children[2]->children[0]->nt, SUBnt);
  TA_INT_EQ(tree->children[2]->children[0]->nchildren, 1);
  TA_INT_EQ(tree->children[2]->children[0]->children[0]->nt, EXPRnt);
  TA_INT_EQ(tree->children[2]->children[0]->children[0]->nchildren, 2);
  TA_INT_EQ(tree->children[2]->children[0]->children[0]->children[0]->nt, TERMnt);
  TA_INT_EQ(tree->children[2]->children[0]->children[0]->children[0]->nchildren, 1);
  TA_INT_EQ(tree->children[2]->children[0]->children[0]->children[0]->children[0]->tok.sym, CharSym);
  TA_CHAR_EQ(tree->children[2]->children[0]->children[0]->children[0]->children[0]->tok.c, 'b');
  TA_INT_EQ(tree->children[2]->children[0]->children[0]->children[1]->tok.sym, Star);

  free_tree(tree);
  return 0;
}

void parse_test(void)
{
  smb_ut_group *group = su_create_test_group("test/parse.c");

  smb_ut_test *TERM_CharSym = su_create_test("TERM_CharSym", test_TERM_CharSym);
  su_add_test(group, TERM_CharSym);

  smb_ut_test *TERM_Minus = su_create_test("TERM_Minus", test_TERM_Minus);
  su_add_test(group, TERM_Minus);

  smb_ut_test *TERM_Caret = su_create_test("TERM_Caret", test_TERM_Caret);
  su_add_test(group, TERM_Caret);

  smb_ut_test *TERM_Dot = su_create_test("TERM_Dot", test_TERM_Dot);
  su_add_test(group, TERM_Dot);

  smb_ut_test *TERM_Special = su_create_test("TERM_Special", test_TERM_Special);
  su_add_test(group, TERM_Special);

  smb_ut_test *TERM_Subexpr = su_create_test("TERM_Subexpr", test_TERM_Subexpr);
  su_add_test(group, TERM_Subexpr);

  smb_ut_test *TERM_Class = su_create_test("TERM_Class", test_TERM_Class);
  su_add_test(group, TERM_Class);

  smb_ut_test *TERM_NClass = su_create_test("TERM_NClass", test_TERM_NClass);
  su_add_test(group, TERM_NClass);

  smb_ut_test *EXPR_Term = su_create_test("EXPR_Term", test_EXPR_Term);
  su_add_test(group, EXPR_Term);

  smb_ut_test *EXPR_Plus = su_create_test("EXPR_Plus", test_EXPR_Plus);
  su_add_test(group, EXPR_Plus);

  smb_ut_test *EXPR_PlusQuestion = su_create_test("EXPR_PlusQuestion", test_EXPR_PlusQuestion);
  su_add_test(group, EXPR_PlusQuestion);

  smb_ut_test *EXPR_Star = su_create_test("EXPR_Star", test_EXPR_Star);
  su_add_test(group, EXPR_Star);

  smb_ut_test *EXPR_StarQuestion = su_create_test("EXPR_StarQuestion", test_EXPR_StarQuestion);
  su_add_test(group, EXPR_StarQuestion);

  smb_ut_test *EXPR_Question = su_create_test("EXPR_Question", test_EXPR_Question);
  su_add_test(group, EXPR_Question);

  smb_ut_test *EXPR_QuestionQuestion = su_create_test("EXPR_QuestionQuestion", test_EXPR_QuestionQuestion);
  su_add_test(group, EXPR_QuestionQuestion);

  smb_ut_test *SUB_Normal = su_create_test("SUB_Normal", test_SUB_Normal);
  su_add_test(group, SUB_Normal);

  smb_ut_test *SUB_Concat = su_create_test("SUB_Concat", test_SUB_Concat);
  su_add_test(group, SUB_Concat);

  smb_ut_test *REGEX_Normal = su_create_test("REGEX_Normal", test_REGEX_Normal);
  su_add_test(group, REGEX_Normal);

  smb_ut_test *REGEX_Alternate = su_create_test("REGEX_Alternate", test_REGEX_Alternate);
  su_add_test(group, REGEX_Alternate);

  smb_ut_test *CLASS_range = su_create_test("CLASS_range", test_CLASS_range);
  su_add_test(group, CLASS_range);

  smb_ut_test *CLASS_range_range = su_create_test("CLASS_range_range", test_CLASS_range_range);
  su_add_test(group, CLASS_range_range);

  smb_ut_test *CLASS_single = su_create_test("CLASS_single", test_CLASS_single);
  su_add_test(group, CLASS_single);

  smb_ut_test *CLASS_single_hyphen = su_create_test("CLASS_single_hyphen", test_CLASS_single_hyphen);
  su_add_test(group, CLASS_single_hyphen);

  smb_ut_test *reparse = su_create_test("reparse", test_reparse);
  su_add_test(group, reparse);

  smb_ut_test *TERM_CharSym_wide = su_create_test("TERM_CharSym_wide", test_TERM_CharSym_wide);
  su_add_test(group, TERM_CharSym_wide);

  smb_ut_test *TERM_Minus_wide = su_create_test("TERM_Minus_wide", test_TERM_Minus_wide);
  su_add_test(group, TERM_Minus_wide);

  smb_ut_test *TERM_Caret_wide = su_create_test("TERM_Caret_wide", test_TERM_Caret_wide);
  su_add_test(group, TERM_Caret_wide);

  smb_ut_test *TERM_Dot_wide = su_create_test("TERM_Dot_wide", test_TERM_Dot_wide);
  su_add_test(group, TERM_Dot_wide);

  smb_ut_test *TERM_Special_wide = su_create_test("TERM_Special_wide", test_TERM_Special_wide);
  su_add_test(group, TERM_Special_wide);

  smb_ut_test *TERM_Subexpr_wide = su_create_test("TERM_Subexpr_wide", test_TERM_Subexpr_wide);
  su_add_test(group, TERM_Subexpr_wide);

  smb_ut_test *TERM_Class_wide = su_create_test("TERM_Class_wide", test_TERM_Class_wide);
  su_add_test(group, TERM_Class_wide);

  smb_ut_test *TERM_NClass_wide = su_create_test("TERM_NClass_wide", test_TERM_NClass_wide);
  su_add_test(group, TERM_NClass_wide);

  smb_ut_test *EXPR_Term_wide = su_create_test("EXPR_Term_wide", test_EXPR_Term_wide);
  su_add_test(group, EXPR_Term_wide);

  smb_ut_test *EXPR_Plus_wide = su_create_test("EXPR_Plus_wide", test_EXPR_Plus_wide);
  su_add_test(group, EXPR_Plus_wide);

  smb_ut_test *EXPR_PlusQuestion_wide = su_create_test("EXPR_PlusQuestion_wide", test_EXPR_PlusQuestion_wide);
  su_add_test(group, EXPR_PlusQuestion_wide);

  smb_ut_test *EXPR_Star_wide = su_create_test("EXPR_Star_wide", test_EXPR_Star_wide);
  su_add_test(group, EXPR_Star_wide);

  smb_ut_test *EXPR_StarQuestion_wide = su_create_test("EXPR_StarQuestion_wide", test_EXPR_StarQuestion_wide);
  su_add_test(group, EXPR_StarQuestion_wide);

  smb_ut_test *EXPR_Question_wide = su_create_test("EXPR_Question_wide", test_EXPR_Question_wide);
  su_add_test(group, EXPR_Question_wide);

  smb_ut_test *EXPR_QuestionQuestion_wide = su_create_test("EXPR_QuestionQuestion_wide", test_EXPR_QuestionQuestion_wide);
  su_add_test(group, EXPR_QuestionQuestion_wide);

  smb_ut_test *SUB_Normal_wide = su_create_test("SUB_Normal_wide", test_SUB_Normal_wide);
  su_add_test(group, SUB_Normal_wide);

  smb_ut_test *SUB_Concat_wide = su_create_test("SUB_Concat_wide", test_SUB_Concat_wide);
  su_add_test(group, SUB_Concat_wide);

  smb_ut_test *REGEX_Normal_wide = su_create_test("REGEX_Normal_wide", test_REGEX_Normal_wide);
  su_add_test(group, REGEX_Normal_wide);

  smb_ut_test *REGEX_Alternate_wide = su_create_test("REGEX_Alternate_wide", test_REGEX_Alternate_wide);
  su_add_test(group, REGEX_Alternate_wide);

  smb_ut_test *CLASS_range_wide = su_create_test("CLASS_range_wide", test_CLASS_range_wide);
  su_add_test(group, CLASS_range_wide);

  smb_ut_test *CLASS_range_range_wide = su_create_test("CLASS_range_range_wide", test_CLASS_range_range_wide);
  su_add_test(group, CLASS_range_range_wide);

  smb_ut_test *CLASS_single_wide = su_create_test("CLASS_single_wide", test_CLASS_single_wide);
  su_add_test(group, CLASS_single_wide);

  smb_ut_test *CLASS_single_hyphen_wide = su_create_test("CLASS_single_hyphen_wide", test_CLASS_single_hyphen_wide);
  su_add_test(group, CLASS_single_hyphen_wide);

  smb_ut_test *reparse_wide = su_create_test("reparse_wide", test_reparse_wide);
  su_add_test(group, reparse_wide);

  su_run_group(group);
  su_delete_group(group);
}
