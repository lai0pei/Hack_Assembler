#include "./main.h"
#define BUFFER 512

extern char *FILE_NAME;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Error parsing Arguments!\n");
        exit(2);
    }

    FILE_NAME = argv[1];
    build_symbol_table();
    code_gen();
    free_table();
    return 0;
}