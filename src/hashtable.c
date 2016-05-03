#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"


typedef struct hashtable table;

table *hashTable_rehash(table *hashtable, int new_capacity);

table *hashTable_Create(int capacity) {
    
    table * hashTable = NULL;
    
    int i;
    
    if(capacity < 1) {
        return NULL;
    }
    
    if((hashTable = malloc(sizeof(table))) == NULL) {
        return NULL;
    }
    
    if( ( hashTable->table = malloc( sizeof( int ) * capacity ) ) == NULL ) {
		return NULL;
	}
    
    for( i = 0; i < capacity; i++ ) {
        hashTable->table[i] = -666;
    }

    hashTable->capacity = capacity;
    hashTable->size = 0;

    return hashTable;	
    
    
}

int ht_hash(table *hashtable, int key ) {

	unsigned long int hashval;
	int i = 0;
        //printf("\n\nin ht HASH");
	hashval = key;
        //printf("\nIn ht_hash()");
	return hashval % hashtable->capacity;
}

table* ht_set( table *hashtable, int key ) {
    
    if(hashtable_contains(hashtable, key) == 0) {
	int bin = 0;
	
	bin = ht_hash( hashtable, key );

	int start = bin - 1;
        
        //printf("\n\tChecking rehash: size: %d\tcapacity: %d\t2.0/3.0: %f", hashtable->size, hashtable->capacity, (2.0/3.0));
        
        if(((double) hashtable->size/ (double) hashtable->capacity) > (2.0/3.0)) {
            //printf("\n\tTIME TO REHASH!!!");
            table *newTable =  hashTable_rehash(hashtable, (hashtable->capacity * 3) + 1);
            hashtable = newTable;
        }
        
        while (bin != start) {
            if(hashtable->table[bin] == -666) {
                hashtable->table[bin] = key;
                break;
            }
            else {
                bin = (bin+1)%hashtable->capacity;
            }
        }
        
        hashtable->size = hashtable->size + 1;
        
        return hashtable;
    }
    else {
        return hashtable;
    }
    

}

table* hashTable_rehash(table *hashtable, int new_capacity) {
    
    table * hashtable2 = hashTable_Create(new_capacity);
    
    int i;
    
    for(i = 0; i < hashtable->capacity; i++) {
        if(hashtable->table[i] != -666) {
            ht_set(hashtable2, hashtable->table[i]);
        }
    }
    
    return hashtable2;
}

int hashtable_contains(table * hashtable, int key) {
    //printf("\n\nTesting2");
    //printf("\n\nTesting3");
    int val = ht_hash(hashtable, key);
    //printf("\n\nTesting4");
    //printf("\n\nTesting5");
    int i = val;
    val = (val - 1) % hashtable->capacity;
    
    if(val == -1) {
        val = hashtable->capacity - 1;
    }
    
    while(i != val) {
        if(hashtable->table[i] == key) {
            return 1;
        }
        else {
            i = ((i + 1)%hashtable->capacity);
        }
    }
    
    return 0;
}