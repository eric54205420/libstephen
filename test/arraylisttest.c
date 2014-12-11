/***************************************************************************//**

  @file         arraylisttest.c

  @author       Stephen Brennan

  @date         Created Saturday, 28 September 2013

  @brief        Test of the array list data structure.

  @copyright    Copyright (c) 2013-2014, Stephen Brennan.
  All rights reserved.

  @copyright
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of Stephen Brennan nor the names of his contributors may
      be used to endorse or promote products derived from this software without
      specific prior written permission.

  @copyright
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL STEPHEN BRENNAN BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#include "libstephen/al.h"
#include "libstephen/ut.h"
#include "tests.h"

////////////////////////////////////////////////////////////////////////////////
// TESTS

int al_test_create()
{
  DATA d;
  smb_status status;
  d.data_llint = 13;

  smb_al *list = al_create(&status);
  TEST_ASSERT(status == SMB_SUCCESS);
  al_append(list, d, &status);
  TEST_ASSERT(status == SMB_SUCCESS);

  TEST_ASSERT(al_length(list) == 1);

  TEST_ASSERT(al_get(list, 0, &status).data_llint == 13);
  TEST_ASSERT(status == SMB_SUCCESS);

  al_delete(list);
  return 0;
}

int al_test_create_empty()
{
  DATA d;
  smb_status status;
  d.data_llint = 13;

  smb_al *list = al_create(&status);

  // Assert that it was allocated correctly.
  TEST_ASSERT(status == SMB_SUCCESS);

  TEST_ASSERT(al_length(list) == 0);

  al_delete(list);
  return 0;
}

int al_test_append()
{
  DATA d;
  smb_status status;
  d.data_llint = 0;

  smb_al *list = al_create(&status);

  // Test append about 21 times to check that reallocation works successfully
  for ( ; d.data_llint < 22; d.data_llint++) {
    al_append(list, d, &status);
    TEST_ASSERT(status == SMB_SUCCESS);

    // Assert that the length of the list is correct
    TEST_ASSERT(al_length(list) == d.data_llint + 1);

    // Assert that each element in the list is correct
    for (int i = 0; i < al_length(list); i++) {
      TEST_ASSERT(al_get(list, i, &status).data_llint == i);
      TEST_ASSERT(status == SMB_SUCCESS);
    }
  }

  al_delete(list);
  return 0;
}

int al_test_prepend()
{
  DATA d;
  smb_status status;
  d.data_llint = 0;

  smb_al *list = al_create(&status);
  TEST_ASSERT(status == SMB_SUCCESS);

  // Test prepend about 21 times to check that reallocation works successfully
  for ( ; d.data_llint < 22; d.data_llint++) {
    al_prepend(list, d, &status);
    TEST_ASSERT(status == SMB_SUCCESS);

    // Assert that the length of the list is correct
    TEST_ASSERT(al_length(list) == d.data_llint + 1);

    // Assert that each element in the list is correct
    for (int i = 0; i < al_length(list); i++) {
      TEST_ASSERT(al_get(list, i, &status).data_llint == d.data_llint - i);
      TEST_ASSERT(status == SMB_SUCCESS);
    }
  }

  al_delete(list);
  return 0;
}

int al_test_set()
{
  DATA d;
  smb_status status;
  smb_al *list = al_create(&status);
  const int length = 30;

  // Create data
  for (int i = 0; i < length; i++) {
    d.data_llint = i;
    al_append(list, d, &status);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Verify data
  for (int i = 0; i < al_length(list); i++) {
    TEST_ASSERT(al_get(list, i, &status).data_llint == i);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Test that the length is correct
  TEST_ASSERT(al_length(list) == length);

  // Test set
  for (int i = 0; i < al_length(list); i++) {
    d.data_llint = al_length(list) - i;
    al_set(list, i , d, &status);
    TEST_ASSERT(status == SMB_SUCCESS);
    TEST_ASSERT(al_get(list, i, &status).data_llint == d.data_llint);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Test that the length is still correct
  TEST_ASSERT(al_length(list) == length);

  al_delete(list);
  return 0;
}

int al_test_remove()
{
  DATA d;
  smb_status status;
  smb_al *list = al_create(&status);
  const int length = 23;
  TEST_ASSERT(status == SMB_SUCCESS);

  // Create data
  for (int i = 0; i < length; i++) {
    d.data_llint = i;
    al_append(list, d, &status);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Verify data
  for (int i = 0; i < al_length(list); i++) {
    TEST_ASSERT(al_get(list, i, &status).data_llint == i);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Remove first
  al_remove(list, 0, &status);
  TEST_ASSERT(status == SMB_SUCCESS);

  TEST_ASSERT(al_get(list, 0, &status).data_llint == 1);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(al_length(list) == length - 1);

  // Remove last
  al_remove(list, al_length(list) - 1, &status);
  TEST_ASSERT(status == SMB_SUCCESS);

  TEST_ASSERT(al_get(list, al_length(list) - 1, &status).data_llint == length - 2);
  TEST_ASSERT(status == SMB_SUCCESS);
  TEST_ASSERT(al_length(list) == length - 2);
  // Current list: 1 2 3 4 5 6 7 8 ...

  // Remove from middle
  al_remove(list, 2, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  // Current list: 1 2 4 5 6 7 8 ...
  TEST_ASSERT(al_length(list) == length - 3);

  // Test all the elements to make sure the data is correct
  int values[] = {1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
  for (int i = 0; i < length - 3; i++) {
    TEST_ASSERT(al_get(list, i, &status).data_llint == values[i]);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Remove the remaining elements, and check that nothing bad happens
  for (int i = length - 4; i >= 0; i--) {
    al_remove(list, 0, &status);
    TEST_ASSERT(al_length(list) == i);
  }

  al_delete(list);
  return 0;
}

int al_test_insert()
{
  DATA d;
  smb_status status;
  smb_al *list = al_create(&status);
  const int length = 20;
  TEST_ASSERT(status == SMB_SUCCESS);

  // Create data
  for (int i = 0; i < length; i++) {
    d.data_llint = i;
    al_append(list, d, &status);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Verify data
  for (int i = 0; i < al_length(list); i++) {
    TEST_ASSERT(al_get(list, i, &status).data_llint == i);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Test insert when a realloc needs to occur
  d.data_llint = 100;
  al_insert(list, 10, d, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  int values_one[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 100, 10, 11, 12, 13, 14,
                      15, 16, 17, 18, 19};
  for (int i = 0; i < al_length(list); i++) {
    TEST_ASSERT(al_get(list, i, &status).data_llint == values_one[i]);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Test insert at end
  d.data_llint++;
  al_insert(list, al_length(list), d, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  int values_two[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 100, 10, 11, 12, 13, 14,
                      15, 16, 17, 18, 19, 101};
  for (int i = 0; i < al_length(list); i++) {
    TEST_ASSERT(al_get(list, i, &status).data_llint == values_two[i]);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  // Test insert at beginning
  d.data_llint++;
  al_insert(list, 0, d, &status);
  TEST_ASSERT(status == SMB_SUCCESS);
  int values_three[] = {102, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 100, 10, 11, 12,
                        13, 14, 15, 16, 17, 18, 19, 101};
  for (int i = 0; i < al_length(list); i++) {
    TEST_ASSERT(al_get(list, i, &status).data_llint == values_three[i]);
    TEST_ASSERT(status == SMB_SUCCESS);
  }

  al_delete(list);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// TEST LOADER AND RUNNER

void array_list_test()
{
  smb_ut_group *group = su_create_test_group("array list");

  smb_ut_test *create = su_create_test("create", al_test_create, 1);
  su_add_test(group, create);

  smb_ut_test *create_empty = su_create_test("create_empty", al_test_create_empty, 1);
  su_add_test(group, create_empty);

  smb_ut_test *append = su_create_test("append", al_test_append, 1);
  su_add_test(group, append);

  smb_ut_test *prepend = su_create_test("prepend", al_test_prepend, 1);
  su_add_test(group, prepend);

  smb_ut_test *set = su_create_test("set", al_test_set, 1);
  su_add_test(group, set);

  smb_ut_test *remove = su_create_test("remove", al_test_remove, 1);
  su_add_test(group, remove);

  smb_ut_test *insert = su_create_test("insert", al_test_insert, 1);
  su_add_test(group, insert);

  // The other elementary operations on the smb_al, i.e. the push, pop and
  // peek functions, are all merely wrappers over the functions already tested.
  // Additionally, the delete and length functions are adequately tested by the
  // tests as they are.  So this is a fairly complete set of tests.

  su_run_group(group);

  su_delete_group(group);
}
