/***************************************************************************//**

  @file         util.c

  @author       Stephen Brennan

  @date         Created Sunday, 1 September 2013

  @brief        Contains definitions for the general purpose items.

  @copyright    Copyright (c) 2013-2016, Stephen Brennan.  Released under the
                Revised BSD License.  See the LICENSE.txt file for details.

*******************************************************************************/

#include <sys/time.h>         /* gettimeofday */
#include <stdio.h>            /* fprintf, fopen */
#include <stdlib.h>           /* malloc, exit */
#include <wchar.h>            /* wchar_t */
#include <stdbool.h>          /* bool */
#include <string.h>           /* strcmp */

#include "libstephen/base.h"  /* SMB_* */


/**
   @brief Utility function for macro smb_new().  Wrapper over malloc().

   Allocate a certain amount of memory.  If allocation fails, EXIT with an error
   message.

   @param amt The number of bytes to allocate.
   @returns The pointer to the allocated memory (guaranteed).
 */
void *smb___new(size_t amt)
{
  void *result = malloc(amt);
  if (!result) {
    fprintf(stderr, "smb_new: allocation error\n");
    exit(1);
  }
  return result;
}

/**
   @brief Utility function for macro smb_renew().  Wrapper over realloc().

   Reallocate a certain amount of memory.

   @param ptr The memory to reallocate.
   @param newsize The new size of the memory.
   @returns The pointer to the new block.
 */
void *smb___renew(void *ptr, size_t newsize)
{
  void *result = realloc(ptr, newsize);
  if (!result) {
    fprintf(stderr, "smb_renew: allocation error\n");
    exit(1);
  }
  return result;
}

/**
   @brief Utility function for macro smb_free().  Wrapper over free().

   Free a pointer.

   @param ptr Memory to free.
 */
void smb___free(void *ptr)
{
  free(ptr);
}

wchar_t *smb_read_linew(FILE *file, smb_status *status)
{
  (void)status; // unused;
  #define SMBRL_BUFSIZE 256
  int bufsize = SMBRL_BUFSIZE;
  int position = 0;
  wchar_t *buffer = smb_new(wchar_t, bufsize);
  wint_t wc;

  while (true) {
    // Read a character
    wc = fgetwc(file);

    // If we hit EOF, replace it with a null character and return.
    if (wc == WEOF || wc == L'\n') {
      buffer[position++] = L'\0';
      return buffer;
    } else {
      buffer[position++] = wc;
    }

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      buffer = smb_renew(wchar_t, buffer, bufsize+SMBRL_BUFSIZE);
      bufsize += SMBRL_BUFSIZE;
    }
  }
}


char *smb_read_line(FILE *file, smb_status *status)
{
  (void) status; // unused
  int bufsize = SMBRL_BUFSIZE;
  int position = 0;
  char *buffer = smb_new(char, bufsize);
  int c;

  while (true) {
    // Read a character
    c = fgetc(file);

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position++] = '\0';
      return buffer;
    } else {
      buffer[position++] = c;
    }

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      buffer = smb_renew(char, buffer, bufsize+SMBRL_BUFSIZE);
      bufsize += SMBRL_BUFSIZE;
    }
  }
}

/**
   @brief Print a DATA, treating it as a `char *`.
   @param f File to print to.
   @param d DATA to print.
 */
void data_printer_string(FILE *f, DATA d)
{
  fprintf(f, "\"%s\"", (char *)d.data_ptr);
}

/**
   @brief Print a DATA, treating it as an int.
   @param f File to print to.
   @param d DATA to print.
 */
void data_printer_int(FILE *f, DATA d)
{
  fprintf(f, "%lld", d.data_llint);
}

/**
   @brief Print a DATA, treating it as a float.
   @param f File to print to.
   @param d DATA to print.
 */
void data_printer_float(FILE *f, DATA d)
{
  fprintf(f, "%f", d.data_dbl);
}

/**
   @brief Print a DATA, treating it as a pointer.
   @param f File to print to.
   @param d DATA to print.
 */
void data_printer_pointer(FILE *f, DATA d)
{
  fprintf(f, "%p", d.data_ptr);
}

/**
   @brief Test whether two null terminated strings are equal.
   @param d1 First data.
   @param d2 Second data.
   @return An integer indicating whether the strings are equal, less than, or
   greater than.
 */
int data_compare_string(DATA d1, DATA d2)
{
  char *s1, *s2;
  s1 = (char *)d1.data_ptr;
  s2 = (char *)d2.data_ptr;
  return strcmp(s1, s2);
}

/**
   @brief Test whether two ints are equal.
   @param d1 First int.
   @param d2 Second int.
   @return An integer indicating whether the ints are equal, less than, or
   greater than.
 */
int data_compare_int(DATA d1, DATA d2)
{
  // Since the difference between two long long ints could be more than an int,
  // we need to store the difference and conditionally return.
  long long int diff = d1.data_llint - d2.data_llint;
  if (diff < 0) {
    return -1;
  } else if (diff > 0) {
    return 1;
  } else {
    return 0;
  }
}

/**
   @brief Test whether two doubles are equal.

   This function compares two doubles stored in DATA.  However, it's NOT a smart
   comparison.  It really just tests whether the binary representations
   themselves are equal.  If you want a smart method for comparing floating
   point numbers, look elsewhere!

   @param d1 First double.
   @param d2 Second double.
   @return An integer indicating whether the doubles are equal, less than, or
   greater than.
 */
int data_compare_float(DATA d1, DATA d2)
{
  double diff = d1.data_llint - d2.data_llint;
  if (diff < 0) {
    return -1;
  } else if (diff > 0) {
    return 1;
  } else {
    return 0;
  }
}

/**
   @brief Test whether two pointers are equal.  Does not order them.
   @param d1 First pointer.
   @param d2 Second pointer.
   @return 0 if equal, 1 if not.
 */
int data_compare_pointer(DATA d1, DATA d2)
{
  if (d1.data_ptr == d2.data_ptr) {
    return 0;
  } else {
    return 1;
  }
}
