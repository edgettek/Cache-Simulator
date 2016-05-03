#include "trace.h"


char ** traceGetLines(char * filename, int lineCount) {
    
    char ** lines = malloc(sizeof(char*) * lineCount * 2);
    
    FILE *trace;
    trace = fopen(filename, "r");
    
    char * loadOrStore = malloc(sizeof(char*) * 100);
    
    char * address = malloc(sizeof(char*) * 100);
    
    int i;
    
    for(i = 0; i < lineCount; i++){
        lines[i] = malloc(sizeof(char*));
    
        fscanf(trace, "%s %s", loadOrStore, address);
        
        lines[i] = strdup(strcat(loadOrStore, address));
        
   }
    
    fclose(trace);
    
  
    return lines;
}

int traceLineCount(char* filename) {
    
    FILE *trace;
    
    trace = fopen(filename, "r");
    char c;
    
    int numLines = 0;
    while((c = fgetc(trace)) != EOF) {
        if(c == '\n') {
            numLines++;
        }
    }
    
    return numLines;
    
}