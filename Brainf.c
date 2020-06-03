//Zenarii 2020
//Brainf*** Interpreter in C
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef uint8_t u8;

#define DEFAULT_STACK_SIZE 1024
#define MAX_FILES_TO_PARSE 64

static struct {
    int OutputStackEachLine;
    int StackSize;
    int FileCount;
    char * FileNames[MAX_FILES_TO_PARSE];
} ProgramOptions;

/*
    File I/O
*/
static char *
LoadEntireFileNT(char * FileName) {
    char * Result = 0;
    FILE * File = fopen(FileName, "rb");
    if(File) {
        fseek(File, 0, SEEK_END);
        int FileSize = ftell(File);
        fseek(File, 0, SEEK_SET);
        Result = malloc(FileSize + 1);
        if(Result) {
            fread(Result, 1, FileSize, File);
            Result[FileSize] = 0;
        }
    }
    else {
        printf("Failed to read file %s.\n", FileName);
    }

    return Result;
}

/*
    Main Program.
*/

static int
InterpretBrainf(char * Stream, u8 * Stack) {
    char * Buffer = Stream;
    size_t Pointer = 0;

    while(*Buffer) {
        switch (*Buffer) {
            case '+': { Stack[Pointer]++; } break;
            case '-': { Stack[Pointer]--; } break;
            case '.': { printf("%c", Stack[Pointer]); } break;
            case ',': { Stack[Pointer] = getchar(); } break;
            case '>': {
                Pointer++;
                if(Pointer > (ProgramOptions.StackSize/sizeof(u8))) {
                    printf("Error: Pointer exceeded stack size (%d)\n", ProgramOptions.StackSize);
                    return 1;
                }
            } break;
            case '<': {
                Pointer--;
                if(Pointer < 0) {
                    printf("Error: Pointer below stack.\n");
                    return 1;
                }
            } break;
            case ']': {
                if(Stack[Pointer]) {
                    int Depth = 0;
                    while(1) {
                        if(*Buffer == '[') Depth--;
                        else if(*Buffer == ']') Depth++;
                        if(*Buffer == '[' && Depth == 0) break;
                        Buffer--;
                    }
                }
            } break;
        }
        Buffer++;
        if(ProgramOptions.OutputStackEachLine) {
            for(int i = 0; i <= Pointer; ++i) {
                printf("%u |", Stack[i]);
            }
        }
    }
    return 0;
}


int
main(int argc, char ** args) {
    ProgramOptions.StackSize = DEFAULT_STACK_SIZE;
    //todo: parse args, want to take a text file and/or a set of commands one after the other
    for(int i = 0; i < argc; ++i) {
        if(strcmp(args[i], "--stack") == 0) {
            int MemSize = atoi(args[i+1]);
            if(MemSize > 0) ProgramOptions.StackSize = MemSize;
            else printf("--stack should be followed by a valid integer greater than 0.\n");
        }
        else if(strcmp(args[i], "--file") == 0) {
            ProgramOptions.FileNames[ProgramOptions.FileCount++] = args[i+1];
        }
        else if(strcmp(args[i], "--debug") == 0) {
            ProgramOptions.OutputStackEachLine = 1;
        }
        else if(strcmp(args[i], "--help") == 0) {
            printf("Usage: %s [Options]\n", args[0]);
            printf("Options:\n");
            printf("\t--stack <size>: Size of the stack used in bytes, defaults to %d\n", DEFAULT_STACK_SIZE);
            printf("\t--file <file path>: Path to a brainfuck file to execute, max files is %d\n", MAX_FILES_TO_PARSE);
            printf("If no files specified, then the program functions as an interpreter and will interpret each line of intput as a seperate brainfuck program\n");
            printf("To exit this mode enter 'exit'.\n");
            return 0;
        }
    }

    u8 * Stack = malloc(ProgramOptions.StackSize * sizeof(u8));
    if(ProgramOptions.FileCount) {
        for(int i = 0; i < ProgramOptions.FileCount; ++i) {
            memset(Stack, 0, ProgramOptions.StackSize/sizeof(u8));
            printf("Interpreting file %s.\n", ProgramOptions.FileNames[i]);
            char * Stream = LoadEntireFileNT(ProgramOptions.FileNames[i]);
            int Error = InterpretBrainf(Stream, Stack);
            if(Error) {
                printf("Terminated execution of brainfuck program due to errors.\n");
            }
        }
    }
    else {
        printf("Entering interpreter mode.\n");
        while(1) {
            memset(Stack, 0, ProgramOptions.StackSize/sizeof(u8));
            char Input[1024] = {0};
            scanf("%s", Input);
            if(strncmp("exit", Input, 4) == 0) break;
            InterpretBrainf(Input, Stack);
            printf("\n");
        }
    }

    printf("Finished program");
    return 0;
}
