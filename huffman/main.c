#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

void printHelp(char *name);
FILE *openFile(const char *name, const char *mode);
long fileSize(FILE *file);
int main(int argc, char *argv[])
{
    if (argc == 4 && (strcmp(argv[1], "encode") == 0 || strcmp(argv[1], "decode") == 0)) {
        FILE *infile = openFile(argv[2], "rb");
        FILE *outfile = openFile(argv[3], "wb");
        if (fileSize(infile) > 0) {
            if (argv[1][0] == 'e') {
                encode(infile, outfile);
            }
            else {
                decode(infile, outfile);
            }
        }
        fclose(infile);
        fclose(outfile);
    }
    else {
        printHelp(argv[0]);
    }
    
    return 0;
}

void printHelp(char *name) {
    printf("usage: %s encode infile outfile\n", name);
    printf("...    %s decode infile outfile\n", name);
}

FILE *openFile(const char *name, const char *mode) {
    FILE *file = fopen(name, mode);
    if (file == NULL) {
        fprintf(stderr, "can't open file %s\n", name);
        exit(1);
    }
    return file;
}

long fileSize(FILE *file) {
    long size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}
