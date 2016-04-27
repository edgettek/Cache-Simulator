#include "trace.h"

// Put anything for reading trace files / writing output files here.

/* TODO: read trace file function */

/* TODO: write output file function */

char ** traceToArray(char * fileName) {
    
    
    
    FILE *trace;
    trace = fopen(fileName, "r");
    
    char c;
    
    int numLines = 0;
    while((c = fgetc(trace)) != EOF) {
        if(c == '\n') {
            numLines++;
        }
    }
    
    printf("Num lines in trace: %d\n", numLines);
    
    char ** lines;
    
    
    fclose(trace);
    
    trace = fopen(fileName, "r");
    
    int i;
    
    char line[200];
    
    printf("\n\nPRINTING LINES FROM TRACE\n");
    
    for(i = 0; i < numLines; i++) {
        fgets(line, sizeof(line), trace);
        
        *(lines+i) = line;
    }
    
    for(i = 0; i < numLines; i++) {
        printf("\nLine of %d: %s", i, *(lines+i));
    }
    
    return lines;
    
    
}


char ** traceGetLines(char * filename) {
    
    FILE *trace;
    
    trace = fopen(filename, "r");
    char c;
    
    int numLines = 0;
    while((c = fgetc(trace)) != EOF) {
        if(c == '\n') {
            numLines++;
        }
    }
    
    printf("Num lines in trace: %d\n", numLines);
    
    fclose(trace);
    
    char ** lines = malloc(sizeof(char*) * numLines * 2);
    
    trace = fopen(filename, "r");
    
    int i;
    
    for(i = 0; i < numLines; i++){
        lines[i] = malloc(sizeof(char*));
    
        fgets(lines[i], sizeof(lines[i]), trace);
        
        
        
        
        printf("line #%d: %s", i, lines[i]);
        
        //printf("\nCounter = %d\n", counter);
   }
    
    
//    int i;
//    for(i = 0; i < counter; i++) {
//        printf("Lines #%d: %s", i, lines[i]);
//    }
        
        fclose(trace);
    
    return lines;
}