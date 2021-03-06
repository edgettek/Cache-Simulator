#include "cache.h"
#include "trace.h"
#include <math.h>

int write_xactions = 0;
int read_xactions = 0;

/*
  Print help message to user
*/
void printHelp(const char * prog) {
	printf("%s [-h] | [-s <size>] [-w <ways>] [-l <line>] [-t <trace>]\n", prog);
	printf("options:\n");
	printf("-h: print out help text\n");
	printf("-s <cache size>: set the total size of the cache in KB\n");
	printf("-w <ways>: set the number of ways in each set\n");
	printf("-l <line size>: set the size of each cache line in bytes\n");
	printf("-t <trace>: use <trace> as the input file for memory traces\n");
  /* EXTRA CREDIT
  printf("-lru: use LRU replacement policy instead of FIFO\n");
  */
}

uint32_t getIndex(int num, int tagNum, int offsetNum) {
    
    uint32_t result = num << tagNum;
    
    //printf("\t\tNum: %x\tTag: %d\tIndex First Shift: %x:\n", num, tagNum, result);
    
    return (uint32_t) (result >> ((tagNum + offsetNum)));
}

uint32_t getTag(int num, int tagNum) {
    
    
    //printf("Num: %x\ttagNum: %d\t(32-tagNum): %d\t", num, tagNum, (32-tagNum));
    
    return (uint32_t) (num >> (32-tagNum));
}

/*
	Main function. Feed to options to set the cache
	
	Options:
	-h : print out help message
	-s : set L1 cache Size (KB)
	-w : set L1 cache ways
	-l : set L1 cache line size
*/
int main(int argc, char* argv[])
{
	int i;
	uint32_t size = 32; //total size of L1$ (KB)
	uint32_t ways = 1; //# of ways in L1. Default to direct-mapped
	uint32_t line = 32; //line size (B)

  // hit and miss counts
  int totalHits = 0;
  int totalMisses = 0;

  char * filename;

	//strings to compare
	const char helpString[] = "-h";
	const char sizeString[] = "-s";
	const char waysString[] = "-w";
	const char lineString[] = "-l";
	const char traceString[] = "-t";
  const char lruString[] = "-lru";
	
  if (argc == 1) {
    // No arguments passed, show help
    printHelp(argv[0]);
    return 1;
  }
  
	//parse command line
	for(i = 1; i < argc; i++)
	{
		//check for help
		if(!strcmp(helpString, argv[i]))
		{
			//print out help text and terminate
                        printHelp(argv[0]);
			return 1; //return 1 for help termination
		}
		//check for size
		else if(!strcmp(sizeString, argv[i]))
		{
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if(isdigit(argv[i][0]))
			{
				size = atoi(argv[i]);
			}
			else
			{
				printf("Incorrect formatting of size value\n");
				return -1; //input failure
			}
		}
		//check for ways
		else if(!strcmp(waysString, argv[i]))
		{
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if(isdigit(argv[i][0]))
			{
				ways = atoi(argv[i]);
			}
			else
			{
				printf("Incorrect formatting of ways value\n");
				return -1; //input failure
			}
		}
		//check for line size
		else if(!strcmp(lineString, argv[i]))
		{
			//take next string and convert to int
			i++; //increment i so that it skips data string in the next loop iteration
			//check next string's first char. If not digit, fail
			if(isdigit(argv[i][0]))
			{
				line = atoi(argv[i]);
			}
			else
			{
				printf("Incorrect formatting of line size value\n");
				return -1; //input failure
			}
		}
    else if (!strcmp(traceString, argv[i])) {
      filename = argv[++i];
    }
    else if (!strcmp(lruString, argv[i])) {
      // Extra Credit: Implement Me!
			printf("Unrecognized argument. Exiting.\n");
			return -1;
    }
		//unrecognized input
		else{
			printf("Unrecognized argument. Exiting.\n");
			return -1;
		}
	}
  
  
  /*
   * NOTES
   * 
   * filename = Name of Trace file
   * 
   * line = Block size
   * 
   * ways = # of ways
   *
   * size = Cache size (KB)
   * 
   */
  
  // INITIALIZE OUTPUT FILE
  
  char outputFileName[100];
  
  FILE *results;
  
  strcpy(outputFileName, filename);
  
  strcat(outputFileName, ".simulated");
  
  results = fopen(outputFileName, "w");

  // TRACE ARRAY
  int instructionCount = traceLineCount(filename);
  
  char ** traceInstr;

  traceInstr = traceGetLines(filename, instructionCount);

  int  traceMiss[sizeof(traceInstr)];
  // CACHE INITIALIZATION
  
  int offset = log2(line); 
  
  int setSize = line * ways;
  
  int numSet = (size * (pow(2,10)))/setSize;
  
  int cache[numSet][(2*ways) + 1];
  
  int k;
  
  for(i = 0; i < numSet; i++) {
      for(k = 0; k < ((2*ways) + 1); k++) {
          cache[i][k] = 0;
          
          if(k == ((2*ways))) {
              cache[i][k] = 1;
          }
          
          //printf("[%d][%d]: %d\t", i, k, cache[i][k]);
      }
      //printf("\n");
  }
  
  int numIndex = log2(numSet);
  
  int numTag = 32 - offset - numIndex;
  
  printf("Ways: %u; Sets: %u; Line Size: %uB\n", ways, numSet, line);
  printf("Tag: %d bits; Index: %d bits; Offset: %d bits\n", numTag, numIndex, offset);
  
  // INITIALIZE ARRAY OF HASHTABLES
  
  table* hashtables = malloc(sizeof(table) * numSet);
  
  for(i = 0; i < numSet; i++) {
      hashtables[i] = * hashTable_Create((double) instructionCount/(double) numSet);
  }
  
  

  // READ TRACE INSTRUCTIONS
  
  char * currentInstr = malloc(sizeof(char*));
  
  int tag, index;
  
  char instruction;
  
  int address;
  
  int lastWay = 0;
  
  int j, z, boolean;

  int cacheSize = ways * numSet;
  
  char* resultString = "uninitialized";
  char outputLine[100];
  
  int booleanConflict = 0;
  
  for(i = 0; i < instructionCount; i++) {
     traceMiss[i] = 0; 
      resultString = "uninitialized";
      
      // ISOLATING INSTRUCTION AND ADDRESS
      
      currentInstr = traceInstr[i];
      //printf("Current Instruction: %s\n", currentInstr);
      
      instruction = currentInstr[0];
      
      currentInstr[0] = '0';

      //printf("\tCurrent Instruction: %s\n", currentInstr);
      
      memmove (currentInstr, currentInstr+1, strlen(currentInstr+1) + 1);
      
      address = (int) strtol(currentInstr, NULL, 0);
      
      //printf("\tAddress: %x\n", address);
      
      tag = getTag(address, numTag);
      
      index = getIndex(address, numTag, offset);
      
      //printf("Tag: %x\tIndex: %x\n", tag, index);
      
      // CHECK IF THE TAG HAS BEEN SEEN BEFORE
      // I.E. IS IT IN THE APPROPRIATE HASHTABLE
      
      boolean = 0;
      booleanConflict = 0;
      
      if(hashtable_contains(&hashtables[index], tag) == 0) {
          resultString = "compulsory";
          boolean = 0;
      }
      else {
          
            boolean = 0;
	    // HIT
            for(z = 1; z < ((2*ways) + 1); z = z+2) {
                if((cache[index][z] == tag) && (cache[index][z-1] == 1)) {
                    resultString = "hit";
                    boolean = 1;
                    totalHits++;
//		    traceMiss[i] = 0;
                    //printf("\nHIT");
                    break;

                }

            }
          
            if(boolean == 0) {
            	// CONFLICT
            	booleanConflict = 0;
		j = i, z = cacheSize;
             	while( j > 0 && z > 0) {
		        int pastAddress = (int) strtol(traceInstr[j], NULL, 0);
			printf("\nj == %d\tpastAddress == %x\ttraceMiss[j] == %d\tz == %d\tcacheSize == %d", j, pastAddress, traceMiss[j], z, cacheSize);
			if( traceMiss[j] == 1 ) {
				printf("\t\tIN IF. Z SHOULD BE DECREASING.");		
				if( getTag(pastAddress, numTag) == tag ) {
					printf("\t\t\t SHOULD BE BREAKING.");
					booleanConflict = 1;
					resultString = "conflict";
					break;
				}
				z = z - 1;
			}

			if( booleanConflict == 1) {
				printf("\t...SO CLOSE.");
				break;
			}

			j = j - 1;
		}
            }
      }
      
//      if(booleanValidBit == 1) {
//          resultString = "conflict";
//      }
      
      if(boolean == 0) {
            lastWay = cache[index][((2*ways))];
            cache[index][lastWay] = tag;
            
            cache[index][lastWay-1] = 1;
            
            lastWay = (lastWay + 2) % ((2*ways));
            totalMisses++;
            
            hashtables[index] = *ht_set(&hashtables[index], tag);
	    traceMiss[i] = 1;
              
      }
      
      if(strcmp(resultString, "uninitialized") == 0) {
          resultString = "capacity";
      }
      
      //sprintf("%c %#010x %s", instruction, address, resultString);
      sprintf(outputLine, "%c 0x%08x %s\n", instruction, address, resultString);
      //resultString = strcat(strcat(instruction, " "), 
      fputs(outputLine, results);
  }
  
  // CLOSE OUTPUT FILE
  
  fclose(results);
  
  
  printf("\n\nHITS: %d\tMisses: %d\n", totalHits, totalMisses);

  /* Print results */
  printf("Miss Rate: %8lf%%\n", ((double) totalMisses) / ((double) totalMisses + (double) totalHits) * 100.0);
  printf("Read Transactions: %d\n", read_xactions);
  printf("Write Transactions: %d\n", write_xactions);

  /* TODO: Now we output the file */

  /* TODO: Cleanup */
}


