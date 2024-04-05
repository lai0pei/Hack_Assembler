#include "../Common.h"
#ifndef PARSER
#define PARSER
//init
void init();
// Instruction Parser api
void file_constructor(void);
bool hasMoreLines(void);
int  advance(void);
void parse_dest(char*);
void parse_comp(char*);
void parse_jump(char*);
void parse_symbol(char*);
void expression(void);
void parser_init(void);
InstructionType instructionType(void);
void clean(void);
void TypeCheck(void);
#endif