#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "hashtable.h"

extern int write_xactions;
extern int read_xactions;

void printHelp(const char * prog);
void initializeCache(int* cache, int length, int width);
