#include "./main.h"
#define BUFFER 512

extern char *FILE_NAME;

int main(int argc, char **argv)
{
    if (argc < 2 || strcmp(argv[1],"-h") == 0)
    {
        fprintf(stdout, "Usage :\n ./assembler Max.asm\n More Info Visit - https://www.nand2tetris.org/ \n Check out Project 6\n");
        exit(0);
    }

    FILE_NAME = argv[1];
    build_symbol_table();
    code_gen();
    free_table();
    return 0;
}