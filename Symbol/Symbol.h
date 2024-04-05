#include "../Common.h"
#ifndef SYMBOL
#define SYMBOL
void addEntry(const char*,const unsigned int);
bool contains(const char*);
unsigned int getAddress(const char*);
void build_symbol_table(void);
void print_table(void);
void free_table(void);
#endif