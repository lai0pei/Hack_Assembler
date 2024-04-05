#include "Code.h"
#include "../Parser/Parser.h"
#include "../Symbol/Symbol.h"
#define SYMBOL_LENGTH 50
#define OUTPUT_FILE "out.hack"
#define OS_SIZE 16
#define STACK_SIZE 50

typedef struct
{
    char *symbol;
    unsigned int value;
} Map;
unsigned int dest(void);
unsigned int comp(void);
unsigned int jump(void);
bool __is_str_alpha(char *);
void __init(void);
void __emit_code();
void __int_to_binary(const unsigned int);
static FILE *W;
static char BIT[OS_SIZE];
static char M = '\0';
static char STACK[STACK_SIZE];
static const Map JUMP[7] = {
    {"JGT", 0b001},
    {"JEQ", 0b010},
    {"JGE", 0b011},
    {"JLT", 0b100},
    {"JNE", 0b101},
    {"JLE", 0b110},
    {"JMP", 0b111},
};
static const Map COMP[28] = {
    {"0", 0b0101010},
    {"1", 0b0111111},
    {"-1", 0b0111010},
    {"D", 0b0001100},
    {"A", 0b0110000},
    {"M", 0b1110000},
    {"!D", 0b0001101},
    {"!A", 0b0110001},
    {"!M", 0b1110001},
    {"-D", 0b0001111},
    {"-A", 0b0110011},
    {"-M", 0b1110011},
    {"D+1", 0b0011111},
    {"A+1", 0b0110111},
    {"M+1", 0b1110111},
    {"D-1", 0b0001110},
    {"A-1", 0b0110010},
    {"M-1", 0b1110010},
    {"D+A", 0b0000010},
    {"D+M", 0b1000010},
    {"D-A", 0b0010011},
    {"D-M", 0b1010011},
    {"A-D", 0b0000111},
    {"M-D", 0b1000111},
    {"D&A", 0b0000000},
    {"D&M", 0b1000000},
    {"D|A", 0b0010101},
    {"D|M", 0b1010101}};

static const unsigned int C_BIN = 0X07 << 13;

extern SubInstructionType SUB_TYPE;
extern FILE *BUFF;
extern unsigned int INDEX;

void code_gen()
{
    W = fopen(OUTPUT_FILE, "w");
    if (W == NULL)
    {
        perror("");
        exit(0);
    }
    char *sym = (char *)malloc(SYMBOL_LENGTH);
    unsigned int a_symbol = 0;

    InstructionType t;
    parser_init();
    file_constructor();
    INDEX = 16;
    while (hasMoreLines())
    {
        if (advance() > 0)
        {
            expression();
            t = instructionType();
          
            if (t == A_INSTRUCTION)
            {
                memset(sym, 0, SYMBOL_LENGTH);
                parse_symbol(sym);
                if (__is_str_alpha(sym))
                {   
                    if(contains(sym)){
                        a_symbol = getAddress(sym);
                    }else{
                        addEntry(sym,INDEX);
                        a_symbol = INDEX;
                        INDEX++;
                    }
                }
                else
                {
                    a_symbol = atoi(sym);
                }
                __int_to_binary(a_symbol);
                __emit_code();
            }

            if (t == C_INSTRUCTION)
            {
                memset(STACK, '\0', STACK_SIZE);
                parse_comp(STACK);
                unsigned int comp_v = comp();
                memset(STACK, '\0', STACK_SIZE);
                parse_dest(STACK);
                unsigned int dest_v = dest();
                memset(STACK, '\0', STACK_SIZE);
                parse_jump(STACK);
                unsigned int jump_v = jump();
                __int_to_binary(C_BIN | comp_v << 6 | dest_v << 3 | jump_v);
                 __emit_code();
            }
           
        }
    }
    fclose(W);
    clean();
}

unsigned int dest()
{
    char t = 0;
    char s = 0;
    char tmp = 0;
   if(strlen(STACK) == 0){
        return 0;
    }
    while(STACK[t] != '\0'){
        switch(STACK[t]){
            case 'A' : 
            s |= 0b100;
            break;
            case 'D' : 
            s |= 0b010;
            break;
            case 'M' : 
            s |= 0b001;
            break;
            default : 
             fprintf(stderr,"Codegen : Invalid Dest!\n");
        exit(0);
        }
        t++;
    }
    return s;
}

unsigned int comp()
{   
    if(strlen(STACK) == 0){
        return 0;
    }
    char t = 0;
    unsigned int d = -1;
    while (COMP[t].symbol != NULL)
    {
        if (strcmp(STACK, COMP[t].symbol) == 0)
        {
            d = COMP[t].value;
            break;
        }
        t++;
    }
    if(d == -1) {
        fprintf(stderr,"Codegen : Invalid Comp!\n");
        exit(0);
    }
        
    return d;
}

unsigned int jump()
{
    char t = 0;
    unsigned int d = -1;
    if(strlen(STACK) == 0){
        return 0;
    }
    while (JUMP[t].symbol != NULL)
    {
        if (strcmp(STACK, JUMP[t].symbol) == 0)
        {
            d = JUMP[t].value;
            break;
        }
        t++;
    }

    if(d == -1) {
        fprintf(stderr,"Codegen : Invalid Jump!\n");
        exit(0);
    }
    return d;
}

bool __is_str_alpha(char *sym)
{
    int t = 0;
    while (sym[t] != '\0')
    {
        if (isalpha(sym[t]))
            return true;

        t++;
    }
    return false;
}

void __int_to_binary(const unsigned int s)
{
    unsigned int divident = s;
    unsigned int modulo = 0;
    char bit = 15;
    memset(BIT, '0', OS_SIZE);

    while (1)
    {
        modulo = divident % 2;
        divident = divident / 2;
        if (modulo > 0)
        {
            BIT[bit] = '1';
        }
        if (divident < 1)
        {
            break;
        }
        bit--;
    }
}

void __emit_code()
{
    char size = 0;
    while (size <= OS_SIZE)
    {
        if (size == OS_SIZE)
        {
            fputc('\n', W);
        }
        else
        {
            fputc(BIT[size], W);
        }
        size++;
    }
}
