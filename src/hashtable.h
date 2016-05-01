#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hashtable {
    int capacity;
    int size;
    int * table;
};

typedef struct hashtable table;

table *hashTable_Create(int capacity);
table* ht_set( table *hashtable, int key );
int hashtable_contains(table * hashtable, int key);

