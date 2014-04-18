/**
  @file    libstephen.h
  
  @author  Stephen Brennan

  @date    Created Friday, 27 September 2013

  @brief   Public interface of libstephen

  Libstephen is a C library designed to provide basic data structures and
  operations to supplement the standard C library.  It is mainly a hobby
  programming project, but on completion it should be usable for basing future
  projects on it.
*/

#ifndef SMB___LIBSTEPHEN_H_
#define SMB___LIBSTEPHEN_H_

#include <stdlib.h>       /* size_t */
#include <stdint.h>       /* uint64_t */

////////////////////////////////////////////////////////////////////////////////
// USEFUL MACROS
////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
    Configuration Section
    Comment/Uncomment the #defines to enable or disable functionality
*******************************************************************************/

#define SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_ENABLE_DIAGNOSTIC_CODE
#define SMB_ENABLE_DIAGNOSTIC_PRINTING

/*******************************************************************************
    SMB_ENABLE_MEMORY_DIAGNOSTICS
    All this functionality is enabled when SMB_MEMORY_DIAGNOSTICS is defined.
*******************************************************************************/

// These functions should NOT be called on their own.  The macros should be
// used, as they are macro controlled.
void smb___helper_inc_malloc_counter(size_t number_of_mallocs);
void smb___helper_dec_malloc_counter(size_t number_of_frees);
size_t smb___helper_get_malloc_counter();

/**
   Call this function with the number of bytes every time you allocate memory.
   Alternatively, call it with the number of times you use the malloc function.
   Whichever policy you use, stick to it.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_INCREMENT_MALLOC_COUNTER(x) smb___helper_inc_malloc_counter(x)
#else
#define SMB_INCREMENT_MALLOC_COUNTER(x)
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/**
   Call this function with the number of bytes every time you free memory.
   Alternatively, call it with the number of times you called free.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_DECREMENT_MALLOC_COUNTER(x) smb___helper_dec_malloc_counter(x)
#else
#define SMB_DECREMENT_MALLOC_COUNTER(x)
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/**
   Call this function to get the number of bytes currently allocated by my code.

   Returns a size_t representing the value of the malloc counter, which could be
   in bytes or mallocs, depending on your convention.
*/
#ifdef SMB_ENABLE_MEMORY_DIAGNOSTICS
#define SMB_GET_MALLOC_COUNTER smb___helper_get_malloc_counter()
#else
#define SMB_GET_MALLOC_COUNTER 0
#endif // SMB_ENABLE_MEMORY_DIAGNOSTICS

/*******************************************************************************
    SMB_ENABLE_DIAGNOSTIC_CODE
    Allows writing diagnostic code.  Enabled by SMB_DIAGNOSTIC_CODE
*******************************************************************************/

/**
   Run the given statement only in diagnostic code.
*/
#ifdef SMB_ENABLE_DIAGNOSTIC_CODE
#define SMB_DIAG_ONLY(x) x
#else
#define SMB_DIAG_ONLY(x)
#endif // SMB_ENABLE_DIAGNOSTIC_CODE

/*******************************************************************************
    SMB_ENABLE_DIAGNOSTIC_PRINTING
*******************************************************************************/

/**
   Pass these parameters to printf only in diagnostic code.  Equivalent to
   SMB_DIAG_ONLY(printf(...)), but SMB_DIAG_PRINT(...) is much nicer.
*/
#ifdef SMB_ENABLE_DIAGNOSTIC_PRINTING
#define SMB_DIAG_PRINT(x...) printf(x)
#else
#define SMB_DIAG_PRINT(x...)
#endif // SMB_ENABLE_DIAGNOSTIC_PRINTING


////////////////////////////////////////////////////////////////////////////////
// ERROR HANDLING
////////////////////////////////////////////////////////////////////////////////

// Not for external use
#define ERROR_VAR smb___error_var_
extern unsigned int ERROR_VAR;

// Error codes //

/**
   ALLOCATION_ERROR: Reserved for communicating errors with malloc().
 */
#define ALLOCATION_ERROR 0x0001

/**
   INDEX_ERROR: Set when an provided index is out of range.  Includes incidents
   when you try to pop or peek at an empty list.
 */
#define INDEX_ERROR 0x0002

/**
   NOT_FOUND_ERROR: Set when an item is not found, mainly in the hash table.
 */
#define NOT_FOUND_ERROR 0x0004

/**
   Set the flag corresponding to the given error code.
 */
#define RAISE(x) (ERROR_VAR |= x)

/**
   Test the flag corresponding to the given error code.  Returns 1 if flag set,
   0 if flag cleared.
 */
#define CHECK(x) (ERROR_VAR & x)

/**
   Clear the flag corresponding to the given error code.
 */
#define CLEAR(x) (ERROR_VAR &= (~x))

/**
   Clear the flags for all errors.
 */
#define CLEAR_ALL_ERRORS ERROR_VAR = 0

////////////////////////////////////////////////////////////////////////////////
// UNIT TESTING
////////////////////////////////////////////////////////////////////////////////

/**
   The size of a description text field (in characters) for an smbunit test.
 */
#define SMB_UNIT_DESCRIPTION_SIZE 20

/**
   The max number of unit tests in a single test group.
 */
#define SMB_UNIT_TESTS_PER_GROUP 20

/**
   Defines a single unit test.  Members should be modified with care, preferably
   not at all, except by using the smbunit functions.

   # Members #
   
   - char description[20]: a 20 character null-terminated string that identifies
     this particular test.

   - int (*run)(): function pointer to the test to run.  The function should
     return 0 if the test is successful.

   - int expected_errors: contains error flags to CHECK() for after execution of
     the test.  The test will fail if at least one of the errors in the flag are
     not raised.
 */
typedef struct smb_ut_test
{
  char description[SMB_UNIT_DESCRIPTION_SIZE];
  int (*run)();
  int expected_errors;
  int check_mem_leaks;
} smb_ut_test;

/**
   A structure holding a group of unit tests that are all related.  Members
   shouldn't me modified by client code.  All should be managed by the functions
   in smbunit.

   # Members #

   - char description[]: a short description (length defined by
     SMB_UNIT_DESCRIPTION_SIZE) for the test.

   - int num_tests: the number of tests in the group.

   - struct smb_ut_test *tests[]: pointers to the tests contained.  Max tests defined by
     SMB_UNIT_TESTS_PER_GROUP.
 */
typedef struct smb_ut_group
{
  char description[SMB_UNIT_DESCRIPTION_SIZE];
  int num_tests;
  smb_ut_test *tests[SMB_UNIT_TESTS_PER_GROUP];

} smb_ut_group;

/**
   Create and return a new unit test.

   # Parameters #

   - char *description: a description of the test.

   - int (*run)(): a function pointer to the test function.

   - int expected_errors: the errors you expect from the test function.  0 if
     non.  You can combine more than one error with &.

   - int check_mem_leaks: whether to check if the mallocs before = mallocs
     after.  0 for no, 1 for yes.

   # Returns #

   A pointer to the new test.
 */
smb_ut_test *su_create_test(char *description, int (*run)(), 
                                   int expected_errors, int check_mem_leaks);

/**
   Create and return a new test group.

   # Parameters #

   - char *description: a short description for the group.

   # Returns #

   A pointer to the test group.
 */
smb_ut_group *su_create_test_group(char *description);

/**
   Add a test to the given test group.  A maximum of SMB_UNIT_TESTS_PER_GROUP
   may be added to the group.  After the limit is reached, this function fails
   *silently*, so as to prevent interference with the actual tests.

   # Parameters #

   - smb_ut_group *group: a pointer to the group to add the test to.

   - smb_ut_test *test: a pointer to the test.
 */
void su_add_test(smb_ut_group *group, smb_ut_test *test);

/**
   Run the given test.  Tracks memory allocations and thrown errors.  In order
   to do this, all errors are cleared before execution of the test.

   # Parameters #
   
   - smb_ut_test *test: the test to run

   # Returns # 
   
   Returns a code based on the execution of the test (and whether or not the
   feature is enabled for the test):

   - Code 0: Test passed all its conditions.

   - Code 1: Test returned a non-zero return code.  This will be reported on
     stdout, along with the specific return code.  The reason for a non-zero
     return code is usually a failed assertion, in which case the code
     corresponds to the assertion number.

   - Code 2: Expected errors not encountered.  The test expected at least one
     error, and none of the expected errors were raised by the function.

   - Code 3: Memory was leaked.  The test returned 0 and all expected errors
     were found (or no errors were expected or found), but memory leaked.
 */
int su_run_test(smb_ut_test *test);

/**
   Run a group of tests.  The tests are run sequentially (in the order they were
   added to the group).  If a test fails, the remaining tests are not executed.

   # Parameters #

   - smb_ut_group *group: a pointer to the smb_ut_group to run.

   # Returns #

   Returns an integer.  Since the tests are run sequentially via the
   su_run_test() function, it returns 0 if all tests succeeded, or else the
   return code of the failed test from su_run_test().
 */
int su_run_group(smb_ut_group *group);

/**
   Frees the memory associated with the test, and performs cleanup.  

   Note that no actual cleanup is required by the test, so the only benefit to
   using this function is that it is future-safe (updates to smbunit may require
   cleanup to be performed in this function), and that it automatically calls
   SMB_DECREMENT_MALLOC_COUNTER().  Failing to call that when freeing a test
   WILL result in a detected memory leak if you place appropriate code in the
   main method, but WILL NOT result in a detected memory leak in any tests.
   Remember that no actual memory leak would have actually occurred.

   # Parameters #

   - smb_ut_test *test: the test to free
 */
void su_delete_test(smb_ut_test *test);

/**
   Free the memory associated with the group AND ALL TESTS WITHIN IT.  You MUST
   use this to delete test groups.  

   Note that if a pointer to a smb_ut_test within the smb_ut_group
   is already invalid (freed), then su_delete_group() assumes that it has been
   freed and moves on.  So you may include a single test in more than one group
   and safely delete them both (but after deleting the first group, the test
   will no longer be valid and a segmentation fault will occur if you try to run
   the second group).

   # Parameters #

   smb_ut_group *group: a pointer to the group to free
 */
void su_delete_group(smb_ut_group *group);

/**
   Asserts that an expression is true.  If false, returns a given value.

   Note that this is a macro, so using some things (++ and -- operators
   especially) can have unintended results.  Any code in expr will be executed
   every time, but code within retval will only be executed when the assertion
   fails.  As a general rule, do not put any complicated code inside the
   TEST_ASSERT() statement at all.

   # PARAMETERS #

   - expr: The expression to check.  If the expression evaluates to true (that
     is, not 0), the assertion passes.  If the expression evaluates to false
     (that is, zero), the assertion fails.

   - retval: An integer return code.  The return code will be given in the test
     output on failure, so follow a convention that makes it easy to identify
     exactly at what point the code fails.
 */
#define TEST_ASSERT(expr, retval) if(!(expr)) return retval

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// DATA TYPE DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
   Generic data type for storage in the data structures.  Each data type takes
   up 8 bytes of memory.
 */
typedef union DATA {
  long long int data_llint;
  double data_dbl;
  void * data_ptr;

} DATA;

/**
   A function pointer that takes a DATA and performs an action on it (counts it,
   calls free on it, prints it, etc.).  Useful for stuff like deleting data
   structures full of items (if they're pointers to dynamically allocated data,
   they'll need to be freed), applying an action to every item in a list
   (e.g. printing), and many more applications.
 */
typedef void (*DATA_ACTION)(DATA toDelete);

////////////////////////////////////////////////////////////////////////////////
// LINKED LIST
////////////////////////////////////////////////////////////////////////////////

/**
   Node structure for linked list.  This must be exposed in order for other data
   types to be public.  This should not be used by users of the library.
 */
typedef struct smb_ll_node
{
  struct smb_ll_node *prev;
  struct smb_ll_node *next;
  DATA data;
} smb_ll_node;

/**
   The actual linked list data type.  "Bare" functions return a pointer to this
   structure.
 */
typedef struct smb_ll
{
  struct smb_ll_node *head;
  struct smb_ll_node *tail;
  int length;
} smb_ll;

/**
   A linked list iterator.  Do not modify the structure yourself.
 */
typedef struct smb_ll_iter
{
  struct smb_ll *list;
  struct smb_ll_node *current;
  int index;
} smb_ll_iter;

///////////////////////////////////////////////////////////////////////////////
// ARRAY LIST
///////////////////////////////////////////////////////////////////////////////

/**
   The actual array list data type.  "Bare" functions return a pointer to this
   structure.  You should not use any of the members, as they are implementation
   specific and subject to change.
 */
typedef struct smb_al
{
  DATA *data;
  int length;
  int allocated;
} smb_al;

////////////////////////////////////////////////////////////////////////////////
// HASH TABLE
////////////////////////////////////////////////////////////////////////////////

/**
   A hash function declaration.
 */
typedef unsigned int (*HASH_FUNCTION)(DATA toHash);

typedef struct smb_ht_bckt
{
  DATA key;
  DATA value;
  struct smb_ht_bckt *next;

} smb_ht_bckt;

typedef struct smb_ht
{
  int length;
  int allocated;
  HASH_FUNCTION hash;
  struct smb_ht_bckt **table;

} smb_ht;

#define HASH_TABLE_INITIAL_SIZE 257 // prime number close to 256
#define HASH_TABLE_MAX_LOAD_FACTOR 0.7

////////////////////////////////////////////////////////////////////////////////
// ARGUMENT DATA
////////////////////////////////////////////////////////////////////////////////

/**
   Data structure to store information on arguments passed to the program.
 */
#define MAX_FLAGS 52
typedef struct smb_ad
{
  uint64_t flags; // bit field for all 52 alphabetical characters
  char *flag_strings[MAX_FLAGS];
  struct smb_ll *long_flags;
  struct smb_ll *long_flag_strings;
  struct smb_ll *bare_strings;

} smb_ad;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// GENERIC TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
   Generic list data structure.
 */
typedef struct smb_list 
{
  // Pointer to the actual structure in memory.  Could be any type.
  void *data;

  // Traditional List functions

  /**
     Append the given data to the given list.

     # Parameters #

     - struct smb_list *l: a pointer to the list to append to

     - DATA newData: the data to add to it.

     # Error Handling #

     Like all main library functions, clears all errors upon entering the
     function.  Can raise an ALLOCATION_ERROR.  On an ALLOCATION_ERROR, the list
     remains valid, but the data is not added to the list.
   */
  void (*append)(struct smb_list *l, DATA newData);

  /**
     Prepend the given data to the beginning of the given list.

     # Parameters #

     - struct smb_list *l: a pointer to the list to append to

     - DATA newData: the data to add to the list

     # Error Handling #

     Clears all errors upon entering the function.  Can raise ALLOCATION_ERROR.
     On an ALLOCATION_ERROR, the list should remain valid, but the data is not
     added to the list.
   */
  void (*prepend)(struct smb_list *l, DATA newData);
  

  /**
     Get the data at the specified index.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     - int index: the index to get

     # Return #

     int: the data at the specified index

     # Error Handling #
     
     Clears all errors on function call.  Can raise an index error.
   */
  DATA (*get)(struct smb_list *l, int index);

  /**
     Set the data at the specified index

     # Parameters # 
     
     - struct smb_list *l: a pointer to the list

     - int index: the index to modify

     - DATA newData: the new data to place at that index

     # Error Handling #

     Clears all errors on function call.  Can raise an index error.
   */
  void (*set)(struct smb_list *l, int index, DATA newData);
  
  /**
     Remove the data at the given index, shifting other entries down.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     - int index: the index to delete

     # Error Handling #

     Clears all errors on function call.  Can raise an index error.
   */
  void (*remove)(struct smb_list *l, int index);

  /**
     Insert data at a specifiend index, moving up all the data at and above that
     index.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     - int index: the index at which to insert

     - DATA newData: the data to insert

     # Error Handling #

     If the index is less than zero, the function prepends.  If the index is
     greater than the size, the function appends.  So no index errors are
     raised.  However, an ALLOCATION_ERROR may be raised.  Clears all errors on
     function call.
   */
  void (*insert)(struct smb_list *l, int index, DATA newData);

  /**
     Delete the entire list.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     # Error Handling #

     No effect on any flags.
   */
  void (*delete)(struct smb_list *l);

  /**
     Return the length of the list.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     # Returns #
     
     An integer, the number of entries in the list.

     # Error Handling #

     No effect on any flags.
   */
  int (*length)(struct smb_list *l);

  // Stack/Queue/Deque Functions

  /**
     "Push" (as in a stack) to the back of the list.  Same as append.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     - DATA newData: data to push

     # Error Handling #

     Clears all errors on a function call. Can raise an allocation error.
   */
  void (*push_back)(struct smb_list *l, DATA newData);

  /**
     "Pop" (as in a stack) from the back of the list.  This returns the value at
     the end of the list and deletes it from the list.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     # Returns #

     The data at the end of the list.

     # Error Handling #

     Clears all errors.  Can raise an INDEX_ERROR.
   */
  DATA (*pop_back)(struct smb_list *l);

  /**
     "Peek" (as in a stack) from the back of the list.  This returns the value
     at the end of the list without deleting it.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     # Returns #

     The data at the end of the list.

     # Error Handling #

     Clears all errors.  Can raise an INDEX_ERROR.
   */
  DATA (*peek_back)(struct smb_list *l);
  

  /**
     "Push" (as in a stack) to the front of the list.  Same as prepend.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     - DATA newData: data to push

     # Error Handling #

     Clears all errors on a function call. Can raise an allocation error.
   */
  void (*push_front)(struct smb_list *l, DATA newData);

  /**
     "Pop" (as in a stack) from the front of the list.  This returns the value
     at the beginning of the list and deletes it from the list.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     # Returns #

     The data at the end of the list.

     # Error Handling #

     Clears all errors.  Can raise an INDEX_ERROR.
   */
  DATA (*pop_front)(struct smb_list *l);

  /**
     "Peek" (as in a stack) from the front of the list.  This returns the value
     at the end of the list without deleting it.

     # Parameters #

     - struct smb_list *l: a pointer to the list

     # Returns #

     The data at the end of the list.

     # Error Handling #

     Clears all errors.  Can raise an INDEX_ERROR.
   */
  DATA (*peek_front)(struct smb_list *l);

  // Iterator functions may reside here in the near future.

} smb_list;

/*******************************************************************************

                              Linked List (smb_ll)

*******************************************************************************/

void ll_init(smb_ll *newList);
smb_ll *ll_create();
void ll_destroy(smb_ll *list);
void ll_delete(smb_ll *list);

smb_list ll_create_list();
smb_list ll_cast_to_list(smb_ll *list);

void ll_append(smb_ll *list, DATA newData);
void ll_prepend(smb_ll *list, DATA newData);
void ll_push_back(smb_ll *list, DATA newData);
DATA ll_pop_back(smb_ll *list);
DATA ll_peek_back(smb_ll *list);
void ll_push_front(smb_ll *list, DATA newData);
DATA ll_pop_front(smb_ll *list);
DATA ll_peek_front(smb_ll *list);
DATA ll_get(smb_ll *list, int index);
void ll_remove(smb_ll *list, int index);
void ll_insert(smb_ll *list, int index, DATA newData);
void ll_set(smb_ll *list, int index, DATA newData);
int ll_length(smb_ll *list);

smb_ll_iter ll_get_iter(smb_ll *list);
DATA ll_iter_next(smb_ll_iter *iterator);
DATA ll_iter_prev(smb_ll_iter *iterator);
DATA ll_iter_curr(smb_ll_iter *iterator);
int ll_iter_has_next(smb_ll_iter *iterator);
int ll_iter_has_prev(smb_ll_iter *iterator);
int ll_iter_valid(smb_ll_iter *iterator);

/*******************************************************************************

                              Array List (smb_al)

*******************************************************************************/

void al_init(smb_al *list);
smb_al *al_create();
void al_destroy(smb_al *list);
void al_delete(smb_al *list);

void al_append(smb_al *list, DATA newData);
void al_prepend(smb_al *list, DATA newData);
DATA al_get(smb_al *list, int index);
void al_remove(smb_al *list, int index);
void al_insert(smb_al *list, int index, DATA newData);
void al_set(smb_al *list, int index, DATA newData);
void al_push_back(smb_al *list, DATA newData);
DATA al_pop_back(smb_al *list);
DATA al_peek_back(smb_al *list);
void al_push_front(smb_al *list, DATA newData);
DATA al_pop_back(smb_al *list);
DATA al_peek_back(smb_al *list);
int al_length(smb_al *list);

/*******************************************************************************

                              Hash Table (smb_ht)

*******************************************************************************/

void ht_init(smb_ht *pTable, HASH_FUNCTION hash_func);
smb_ht *ht_create(HASH_FUNCTION hash_func);
void ht_destroy_act(smb_ht *pTable, DATA_ACTION deleter);
void ht_destroy(smb_ht *pTable);
void ht_delete_act(smb_ht *pTable, DATA_ACTION deleter);
void ht_delete(smb_ht *pTable);

void ht_insert(smb_ht *pTable, DATA dKey, DATA dValue);
void ht_remove_act(smb_ht *pTable, DATA dKey, DATA_ACTION deleter);
void ht_remove(smb_ht *pTable, DATA dKey);
DATA ht_get(smb_ht const *pTable, DATA dKey);
unsigned int ht_string_hash(DATA data);
void ht_print(smb_ht const *pTable, int full_mode);

////////////////////////////////////////////////////////////////////////////////
// ARGUMENT DATA
////////////////////////////////////////////////////////////////////////////////

void arg_data_init(smb_ad *data);
smb_ad *arg_data_create();
void arg_data_destroy(smb_ad *data);
void arg_data_delete(smb_ad *data);

/**
   Analyze the argument data passed to the program.  Pass in the argc and argv,
   but make sure to decrement and increment each respective variable so they do
   not include the name of the program.

   # Parameters #

   - int argc: The number of arguments (not including program name).

   - char **argv: The arguments themselves (not including program name).

   # Return #

   A pointer to an smb_ad object.  Use provided functions to query the object
   about every desired flag.
 */
void process_args(smb_ad *data, int argc, char **argv);

/**
   Check whether a flag is raised.

   # Parameters #

   - smb_ad *data: The smb_ad returned by process_args().

   - char flag: The character flag to check.  Alphabetical only.

   # Return #

   An integer, 0 iff the flag was not set.
 */
int check_flag(smb_ad *data, char flag);

/**
   Check whether a long flag appeared.  It must occur verbatim.

   # Parameters #

   - smb_ad *data: The smb_ad returned by process_args().

   - char *flag: The string flag to check for.

   # Return #

   An integer, 0 iff the flag was not set.
 */
int check_long_flag(smb_ad *data, char *flag);

/**
   Check whether a bare string appeared.  It must occur verbatim.

   # Parameters #

   - smb_ad *data: The smb_ad returned by process_args().

   - char *string: The string to search for.
 */
int check_bare_string(smb_ad *data, char *string);

/**
   Return the string parameter associated with the flag.

   # Parameters #

   - smb_ad *data: The smb_ad returned by process_args().

   - char flag: The flag to find parameters of.

   # Return #

   The parameter of the flag.
 */
char *get_flag_parameter(smb_ad *data, char flag);

/**
   Return the string parameter associated with the long string.

   # Parameters #

   - smb_ad *data: The smb_ad returned by process_args().

   - char *string: The long flag to find parameters of.

   # Return #

   The parameter of the long flag.  NULL if no parameter or if flag not found.
 */
char *get_long_flag_parameter(smb_ad *data, char *string);

////////////////////////////////////////////////////////////////////////////////
// Bit Field

#define BIT_PER_CHAR 8
#define SMB_BITFIELD_SIZE(num_bools) ((int)((num_bools) / BIT_PER_CHAR) + \
                                      ((num_bools) % BIT_PER_CHAR == 0 ? 0 : 1))

/**
   Allocate a bitfield capable of holding the given number of bools.  Will be
   allocated in dynamic memory, and a pointer will be returned.

   # Parameters #

   - int num_bools: The number of bools to fit in the bit field.

   # Return #

   A pointer to the bitfield.

   # Error Handling #

   Clears all errors on function call.  If the memory cannot be allocated,
   raises ALLOCATION_ERROR.
 */
unsigned char *bf_create(int num_bools);

/**
   Delete the bitfield pointed to.  Only do this if you created the bitfield via
   bf_create().

   # Parameters #

   - unsigned char *data: A pointer to the bitfield.

   - int num_bools: The number of bools contained in the bitfield.

   # Error Handling #

   No effect.
 */
void bf_delete(unsigned char *data, int num_bools);

/**
   Initialize the memory where a bitfield is contained to all 0's.  This is
   public so people can use the function to allocate their own bitfields on
   function stacks instead of via the heap.

   # Parameters #

   - unsigned char *data: A pointer to the bitfield.

   - int num_bools: The size of the bitfield, in number of bools (aka bits, not
     bytes).

   # Error Handling #
   
   No effect
 */
void bf_init(unsigned char *data, int num_bools);

/**
   Check whether the given bit is set.

   # Parameters #

   - unsigned char *data: A pointer to the bitfield.

   - int index: The index of the bit to Check

   # Return #

   0 if the bit is not set.  Non zero if the bit is set.

   # Error Handling #

   No effect.
 */
int bf_check(unsigned char *data, int index);

/**
   Set a bit.

   # Parameters #

   - unsigned char *data: A pointer to the bitfield

   - int index: The index of the bit to set.

   # Error Handling #

   No effect.
 */
void bf_set(unsigned char *data, int index);

/**
   Clear a bit.

   # Parameters #

   - unsigned char *data: A pointer to the bitfield.

   - int index: The index of the bit to clear.

   # Error Handling #

   No effect.
 */
void bf_clear(unsigned char *data, int index);

/**
   Clear a bit.

   # Parameters #

   - unsigned char *data: A pointer to the bitfield.

   - int index: The index of the bit to flip.

   # Error Handling #

   No effect.
 */
void bf_flip(unsigned char *data, int index);

#endif // SMB___LIBSTEPHEN_H_
