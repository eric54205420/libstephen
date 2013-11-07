/*******************************************************************************

  File:         hashtable.c

  Author:       Stephen Brennan

  Date Created: Thursday,  7 November 2013

  Description:  A simple hash table, with an included hash function for strings.

*******************************************************************************/

#include <libstephen.h>

////////////////////////////////////////////////////////////////////////////////
// Data structure definitions
////////////////////////////////////////////////////////////////////////////////

#define HT_BUCKET struct ht_bucket
HT_BUCKET
{
  DATA key;
  DATA value;
  HT_BUCKET *next;
};

#define HASH_TABLE struct hash_table
HASH_TABLE
{
  int length;
  int allocated;
  int (*hash)(DATA dData);
  HT_BUCKET **table;
};

#define HASH_TABLE_INITIAL_SIZE 256

////////////////////////////////////////////////////////////////////////////////
// Private Helper Functions
////////////////////////////////////////////////////////////////////////////////

HT_BUCKET *ht_bucket_create(DATA dKey, DATA dValue, HT_BUCKET const *pNext)
{
  HT_BUCKET *pBucket;
  pBucket = (HT_BUCKET*) malloc(sizeof(HT_BUCKET));
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(HT_BUCKET));
  pBucket->key = dKey;
  pBucket->value = dValue;
  pBucket->next = pNext;
  return pBucket;
}

/*
  Find a pointer to the first hash entry with key==dKey.  If it doesn't exist,
  return a pointer to the tail of the list.  If the bucket is null, return NULL.
 */
HT_BUCKET *ht_find_in_bucket(HT_BUCKET *pBucket, DATA dKey)
{
  if (pBucket == NULL) return NULL;

  while (pBucket->next && pBucket->key != dKey) pBucket = pBucket->next;

  return pBucket;
}

////////////////////////////////////////////////////////////////////////////////
// Public Interface Functions
////////////////////////////////////////////////////////////////////////////////

HASH_TABLE *ht_create(int (*hash_function)(DATA dData))
{
  // Allocate and create the table.
  HASH_TABLE *pTable;
  pTable = (HASH_TABLE*) malloc(sizeof(HASH_TABLE));
  SMB_INCREMENT_MALLOC_COUNTER(sizeof(HASH_TABLE));

  // Initialize values
  pTable->length = 0;
  pTable->allocated = HASH_TABLE_INITIAL_SIZE;
  pTable->hash = hash_function;
  pTable->table = (HT_BUCKET**) malloc(HASH_TABLE_INITIAL_SIZE * sizeof(HT_BUCKET*));
  SMB_INCREMENT_MALLOC_COUNTER(HASH_TABLE_INITIAL_SIZE * sizeof(HT_BUCKET*));

  // Zero out the entries in the table so we don't get segmentation faults.
  memset(pTable->table, 0, HASH_TABLE_INITIAL_SIZE * sizeof(HT_BUCKET*));

  return pTable;
}

void ht_insert(HASH_TABLE *pTable, DATA dKey, DATA dValue)
{
  HT_BUCKET *curr;
  int index = pTable->hash(dKey) % pTable->allocated;
  
  if (pTable->table[index]) {
    // A linked list already exists here.
    curr = ht_find_in_bucket(pTable->table[index], dKey);
    if (curr->key == dKey) {
      curr->value = dValue;
    } else {
      // assert curr->next == NULL
      curr->next = ht_bucket_create(dKey, dValue, NULL);
    }
  } else {
    // No linked list exists yet
    pTable->table[index] = ht_bucket_create(dKey, dValue, NULL);
  }
}

void ht_remove(HASH_TABLE *pTable, DATA dKey)
{
  HT_BUCKET *curr, *prev;
  int index = pTable->hash(dKey) % pTable->allocated;

  
}

 ht_get(HASH_TABLE *pTable, DATA dKey)
{

}
