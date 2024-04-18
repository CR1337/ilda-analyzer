#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "model.h"
#include "analyze.h"

void printUsage(char executableName[]) {
    fprintf(stderr, "Usage: %s <INPUT_FILE>\n", executableName);
}

void parseInputFilename(int argc, char *argv[], char **inputFilename) {
    if (argc != 2) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }
    *inputFilename = argv[1];
}

int main(int argc, char *argv[]) {
    char *inputFilename = NULL;
    parseInputFilename(argc, argv, &inputFilename);

    FILE *inputFile = fopen(inputFilename, "rb");
    if (!inputFile) {
        fprintf(stderr, "Error: Could not open file %s\n", inputFilename);
        return EXIT_FAILURE;
    }

    analyze(inputFile);

    fclose(inputFile);
    return EXIT_SUCCESS;
}
