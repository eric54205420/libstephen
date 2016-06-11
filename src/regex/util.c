/***************************************************************************//**

  @file         util.c

  @author       Stephen Brennan

  @date         Created Sunday, 29 May 2016

  @brief        Regex handling utilities.

  @copyright    Copyright (c) 2016, Stephen Brennan.  Released under the
                Revised BSD License.  See LICENSE.txt for details.

*******************************************************************************/

#include <string.h>

#include "libstephen/re.h"
#include "libstephen/re_internals.h"

Captures recap(const char *s, const size_t *l, size_t n)
{
  Captures c;
  c.n = n/2;
  c.cap = calloc(c.n, sizeof(char *));
  for (size_t i = 0; i < n / 2; i++) {
    size_t start = l[i*2];
    size_t end = l[i*2 + 1];
    size_t length = end - start + 1;
    c.cap[i] = malloc(end - start + 1);
    strncpy(c.cap[i], s + start, length);
    c.cap[i][length-1] = '\0';
  }
  return c;
}

void recapfree(Captures c)
{
  for (size_t i = 0; i < c.n; i++) {
    free(c.cap[i]);
  }
  free(c.cap);
}