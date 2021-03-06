/***************************************************************************//**

  @file         charbuftest.c

  @author       Stephen Brennan

  @date         Created Saturday, 23 May 2015

  @brief        Tests for the charbuf.

  @copyright    Copyright (c) 2015-2016, Stephen Brennan.  Released under the
                Revised BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <string.h>
#include <wchar.h>

#include "libstephen/cb.h"
#include "libstephen/ut.h"

/**
   @brief Ensure that simple operation has no memory leaks.
 */
int test_cbuf_memory(void)
{
  cbuf *c = cb_create(10);
  cb_delete(c);
  return 0;
}

/**
   @brief Ensure that simple operation has no memory leaks.
 */
int test_wcbuf_memory(void)
{
  wcbuf *wc = wcb_create(10);
  wcb_delete(wc);
  return 0;
}

/**
   @brief Test concatenating to a cbuf with no allocation.
 */
int test_cbuf_concat_noalloc(void)
{
  cbuf *c = cb_create(10);
  cb_concat(c, "abc");
  cb_concat(c, "def");
  TA_STR_EQ(c->buf, "abcdef");
  TA_INT_EQ(c->capacity, 10);
  TA_INT_EQ(c->length, 6);
  cb_delete(c);
  return 0;
}

/**
   @brief Test concatenating to a wcbuf with no reallocation.
 */
int test_wcbuf_concat_noalloc(void)
{
  wcbuf *wc = wcb_create(10);
  wcb_concat(wc, L"abc");
  wcb_concat(wc, L"def");
  TA_WSTR_EQ(wc->buf, L"abcdef");
  TA_INT_EQ(wc->capacity, 10);
  TA_INT_EQ(wc->length, 6);
  wcb_delete(wc);
  return 0;
}

/**
   @brief Test concatenating to a cbuf with reallocation.
 */
int test_cbuf_concat_realloc(void)
{
  cbuf *c = cb_create(4);
  cb_concat(c, "abc");
  cb_concat(c, "def");
  TA_STR_EQ(c->buf, "abcdef");
  TA_INT_EQ(c->capacity, 8);
  TA_INT_EQ(c->length, 6);
  cb_delete(c);
  return 0;
}

/**
   @brief Test concatenating to a wcbuf with reallocation.
 */
int test_wcbuf_concat_realloc(void)
{
  wcbuf *wc = wcb_create(4);
  wcb_concat(wc, L"abc");
  wcb_concat(wc, L"def");
  TA_WSTR_EQ(wc->buf, L"abcdef");
  TA_INT_EQ(wc->capacity, 8);
  TA_INT_EQ(wc->length, 6);
  wcb_delete(wc);
  return 0;
}

/**
   @brief Test appending a character to a cbuf without reallocation.
 */
int test_cbuf_append_noalloc(void)
{
  cbuf *c = cb_create(4);
  cb_append(c, 'a');
  cb_append(c, 'b');
  cb_append(c, 'c');
  TA_STR_EQ(c->buf, "abc");
  TA_INT_EQ(c->capacity, 4);
  TA_INT_EQ(c->length, 3);
  cb_delete(c);
  return 0;
}

/**
   @brief Test appending a character to a wcbuf without reallocation.
 */
int test_wcbuf_append_noalloc(void)
{
  wcbuf *wc = wcb_create(4);
  wcb_append(wc, L'a');
  wcb_append(wc, L'b');
  wcb_append(wc, L'c');
  TA_WSTR_EQ(wc->buf, L"abc");
  TA_INT_EQ(wc->capacity, 4);
  TA_INT_EQ(wc->length, 3);
  wcb_delete(wc);
  return 0;
}

/**
   @brief Test appending a character to a cbuf with reallocation.
 */
int test_cbuf_append_realloc(void)
{
  cbuf *c = cb_create(3);
  cb_append(c, 'a');
  cb_append(c, 'b');
  cb_append(c, 'c');
  TA_STR_EQ(c->buf, "abc");
  TA_INT_EQ(c->capacity, 6);
  TA_INT_EQ(c->length, 3);
  cb_delete(c);
  return 0;
}

/**
   @brief Test appending a character to a wcbuf with reallocation.
 */
int test_wcbuf_append_realloc(void)
{
  wcbuf *wc = wcb_create(3);
  wcb_append(wc, L'a');
  wcb_append(wc, L'b');
  wcb_append(wc, L'c');
  TA_WSTR_EQ(wc->buf, L"abc");
  TA_INT_EQ(wc->capacity, 6);
  TA_INT_EQ(wc->length, 3);
  wcb_delete(wc);
  return 0;
}

/**
   @brief Test printf to a cbuf.
 */
int test_cbuf_printf(void)
{
  cbuf *cb = cb_create(8);
  cb_concat(cb, "prefix ");
  cb_printf(cb, "format %ls %s %d suffix", L"wcs", "mbs", 20);
  TA_STR_EQ(cb->buf, "prefix format wcs mbs 20 suffix");
  cb_delete(cb);
  return 0;
}

/**
   @brief Test printf to a wcbuf.
 */
int test_wcbuf_printf(void)
{
  wcbuf *wcb = wcb_create(8);
  wcb_concat(wcb, L"prefix ");
  wcb_printf(wcb, L"format %ls %s %d suffix", L"wcs", "mbs", 20);
  TA_WSTR_EQ(wcb->buf, L"prefix format wcs mbs 20 suffix");
  wcb_delete(wcb);
  return 0;
}

/**
   @brief Test trimming a cbuf.
 */
int test_cbuf_trim(void)
{
  cbuf *cb = cb_create(8);
  cb_concat(cb, "abc");
  TA_INT_EQ(cb->capacity, 8);
  TA_INT_EQ(cb->length, 3);
  cb_trim(cb);
  TA_INT_EQ(cb->capacity, 4);
  TA_INT_EQ(cb->length, 3);
  cb_delete(cb);
  return 0;
}

/**
   @brief Test trimming a wcbuf.
 */
int test_wcbuf_trim(void)
{
  wcbuf *wcb = wcb_create(8);
  wcb_concat(wcb, L"abc");
  TA_INT_EQ(wcb->capacity, 8);
  TA_INT_EQ(wcb->length, 3);
  wcb_trim(wcb);
  TA_INT_EQ(wcb->capacity, 4);
  TA_INT_EQ(wcb->length, 3);
  wcb_delete(wcb);
  return 0;
}

/**
   @brief Test clearing a cbuf.
 */
int test_cbuf_clear(void)
{
  cbuf *cb = cb_create(8);
  cb_concat(cb, "abc");
  TA_STR_EQ(cb->buf, "abc");
  cb_clear(cb);
  TA_STR_EQ(cb->buf, "");
  cb_delete(cb);
  return 0;
}

/**
   @brief Test clearing a wcbuf.
 */
int test_wcbuf_clear(void)
{
  wcbuf *wcb = wcb_create(8);
  wcb_concat(wcb, L"abc");
  TA_WSTR_EQ(wcb->buf, L"abc");
  wcb_clear(wcb);
  TA_WSTR_EQ(wcb->buf, L"");
  wcb_delete(wcb);
  return 0;
}

void charbuf_test(void)
{
  smb_ut_group *group = su_create_test_group("test/charbuftest.c");

  smb_ut_test *cbuf_memory = su_create_test("cbuf_memory", test_cbuf_memory);
  su_add_test(group, cbuf_memory);

  smb_ut_test *wcbuf_memory = su_create_test("wcbuf_memory", test_wcbuf_memory);
  su_add_test(group, wcbuf_memory);

  smb_ut_test *cbuf_concat_noalloc = su_create_test("cbuf_concat_noalloc", test_cbuf_concat_noalloc);
  su_add_test(group, cbuf_concat_noalloc);

  smb_ut_test *wcbuf_concat_noalloc = su_create_test("wcbuf_concat_noalloc", test_wcbuf_concat_noalloc);
  su_add_test(group, wcbuf_concat_noalloc);

  smb_ut_test *cbuf_concat_realloc = su_create_test("cbuf_concat_realloc", test_cbuf_concat_realloc);
  su_add_test(group, cbuf_concat_realloc);

  smb_ut_test *wcbuf_concat_realloc = su_create_test("wcbuf_concat_realloc", test_wcbuf_concat_realloc);
  su_add_test(group, wcbuf_concat_realloc);

  smb_ut_test *cbuf_printf = su_create_test("cbuf_printf", test_cbuf_printf);
  su_add_test(group, cbuf_printf);

  smb_ut_test *wcbuf_printf = su_create_test("wcbuf_printf", test_wcbuf_printf);
  su_add_test(group, wcbuf_printf);

  smb_ut_test *cbuf_append_noalloc = su_create_test("cbuf_append_noalloc", test_cbuf_append_noalloc);
  su_add_test(group, cbuf_append_noalloc);

  smb_ut_test *wcbuf_append_noalloc = su_create_test("wcbuf_append_noalloc", test_wcbuf_append_noalloc);
  su_add_test(group, wcbuf_append_noalloc);

  smb_ut_test *cbuf_append_realloc = su_create_test("cbuf_append_realloc", test_cbuf_append_realloc);
  su_add_test(group, cbuf_append_realloc);

  smb_ut_test *wcbuf_append_realloc = su_create_test("wcbuf_append_realloc", test_wcbuf_append_realloc);
  su_add_test(group, wcbuf_append_realloc);

  smb_ut_test *cbuf_trim = su_create_test("cbuf_trim", test_cbuf_trim);
  su_add_test(group, cbuf_trim);

  smb_ut_test *wcbuf_trim = su_create_test("wcbuf_trim", test_wcbuf_trim);
  su_add_test(group, wcbuf_trim);

  smb_ut_test *cbuf_clear = su_create_test("cbuf_clear", test_cbuf_clear);
  su_add_test(group, cbuf_clear);

  smb_ut_test *wcbuf_clear = su_create_test("wcbuf_clear", test_wcbuf_clear);
  su_add_test(group, wcbuf_clear);

  su_run_group(group);
  su_delete_group(group);
}
