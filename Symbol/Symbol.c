#include "Symbol.h"
#include "../Parser/Parser.h"
#define BUCKET_SIZE 2048
#define SYMBOL_LENGTH 50
#define DEFAULT_MAP 18

extern const unsigned int L_INSTRUCTION_LINES;
extern const InstructionType TYPE;

typedef unsigned int HashInt;

typedef struct
{
    char symbol[SYMBOL_LENGTH];
    unsigned int address;
} Symbol;

typedef struct
{
    Symbol *symbol;
} Storage;

Symbol MEMORY_MAP[DEFAULT_MAP] = {
    {"R0", 0},
    {"R1", 1},
    {"R2", 2},
    {"R3", 3},
    {"R4", 4},
    {"R5", 5},
    {"R6", 6},
    {"R7", 7},
    {"R8", 8},
    {"R9", 9},
    {"R10", 10},
    {"R11", 11},
    {"R12", 12},
    {"R13", 13},
    {"R14", 14},
    {"R15", 15},
    {"SCREEN", 16384},
    {"KDB", 24576},
};

static Storage *H = NULL;
unsigned int INDEX = DEFAULT_MAP;
static bool __is_str_alpha(const char *);
HashInt __find_slot(const char *);
HashInt __hash_index(const char *);

void build_symbol_table()
{
    file_constructor();
    H = (Storage *)malloc(sizeof(Storage) * BUCKET_SIZE);

    if (H == NULL)
    {
        perror("");
        exit(0);
    }
    memset(H, 0, BUCKET_SIZE);
    char t = 0;
    Symbol *sym = NULL;
    while (t < DEFAULT_MAP)
    {
        addEntry(MEMORY_MAP[t].symbol, MEMORY_MAP[t].address);
        t++;
    }
    char *str = (char *)malloc(sizeof(char) * SYMBOL_LENGTH);
    parser_init();

    while (hasMoreLines())
    {
        if (advance() > 0)
        {
            expression();
            if (TYPE == L_INSTRUCTION)
            {
                memset(str, 0, sizeof(char) * SYMBOL_LENGTH);
                parse_symbol(str);

                if (!contains(str))
                {
                    addEntry(str, L_INSTRUCTION_LINES);
                }
            }
        }
    }
    clean();
}

void print_table()
{
    HashInt c = 0;
    HashInt t = 0;
    while (c < BUCKET_SIZE)
    {
        if (H[c].symbol != NULL)
        {
            printf("%s - %d \n", H[c].symbol->symbol, H[c].symbol->address);
            t++;
        }
        c++;
    }
    printf("Total in table %d\n", t);
}

void addEntry(const char *symbol, const unsigned int address)
{
    if (H == NULL)
    {
        fprintf(stderr, "create table first\n");
        exit(0);
    }
    if (strlen(symbol) > SYMBOL_LENGTH)
    {
        fprintf(stderr, "Only %d symbol length allowed\n", SYMBOL_LENGTH);
        exit(0);
    }
    HashInt hash = __hash_index(symbol);
    Symbol *sym = NULL;
    sym = (Symbol *)malloc(sizeof(Symbol));
    memset(sym, 0, sizeof(Symbol));
    strcpy(sym->symbol, symbol);
    sym->address = address;
    if (H[hash].symbol != NULL)
    {
        hash = __find_slot(symbol);
    }
    H[hash].symbol = sym;
}

// opening addressing hashing
HashInt __hash_index(const char *symbol)
{
    char c = 0;
    HashInt key = 0;
    while (symbol[c] != '\0')
    {
        key += symbol[c];
        c++;
    }
    return key % BUCKET_SIZE;
}

unsigned int getAddress(const char *symbol)
{
    unsigned int i = __hash_index(symbol);

    if (H[i].symbol == NULL)
    {
        fprintf(stderr, "No symbol address found %s!\n", symbol);
        exit(0);
    }

    while (H[i].symbol != NULL)
    {
        if (strcmp(symbol, H[i].symbol->symbol) == 0)
        {
            return H[i].symbol->address;
        }
        i++;
    }

    return H[i].symbol->address;
}

HashInt __find_slot(const char *symbol)
{
    HashInt i = __hash_index(symbol);
    while (H[i].symbol != NULL)
    {
        i++;
    }
    return i % BUCKET_SIZE;
}

bool contains(const char *symbol)
{
    unsigned int i = __hash_index(symbol);
    while (H[i].symbol != NULL)
    {
        if (strcmp(symbol, H[i].symbol->symbol) == 0)
        {
            return true;
        }
        i++;
    }
    return false;
}

bool __is_str_alpha(const char *sym)
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

void free_table()
{
    HashInt c = 0;
    HashInt t = 0;
    while (c < BUCKET_SIZE)
    {
        if (H[c].symbol != NULL)
        {
            free(H[c].symbol);
            t++;
        }
        c++;
    }
}