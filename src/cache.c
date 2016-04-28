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
  

  // TRACE ARRAY
  int instructionCount = traceLineCount(filename);
  
  char ** traceInstr;

  traceInstr = traceGetLines(filename, instructionCount);
  

  // CACHE INITIALIZATION
  
  int offset = log2(line); 
  
  int setSize = line * ways;
  
  int numSet = (size * (pow(2,10)))/setSize;
  
  int cache[numSet][(2*ways) + 1];
  
  int k;
  
  for(i = 0; i < numSet; i++) {
      for(k = 0; k < ((2*ways) + 1); k++) {
          cache[i][k] = 0;
          
          //printf("[%d][%d]: %d\t", i, k, cache[i][k]);
      }
      //printf("\n");
  }
  
  int numIndex = log2(numSet);
  
  int numTag = 32 - offset - numIndex;
  
  printf("Ways: %u; Sets: %u; Line Size: %uB\n", ways, numSet, line);
  printf("Tag: %d bits; Index: %d bits; Offset: %d bits\n", numTag, numIndex, offset);

  // READ TRACE INSTRUCTIONS
  
  char * currentInstr = malloc(sizeof(char*));
  
  int tag, index;
  
  char instruction;
  
  int address;
  
  for(i = 0; i < 30; i++) {
      
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
      
      //
      
      if(instruction == 's') {
          write_xactions++;
      }
      else {
          read_xactions++;
      }
      
      printf("\ninstruction: %c\twrite_xactions: %d\tread_xactions: %d\t", instruction, write_xactions, read_xactions);
      
      
      
  }
  
  

  /* Print results */
  printf("Miss Rate: %8lf%%\n", ((double) totalMisses) / ((double) totalMisses + (double) totalHits) * 100.0);
  printf("Read Transactions: %d\n", read_xactions);
  printf("Write Transactions: %d\n", write_xactions);

  /* TODO: Now we output the file */

  /* TODO: Cleanup */
}


