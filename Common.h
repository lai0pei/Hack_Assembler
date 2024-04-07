#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <limits.h>
#ifndef API
#define API
typedef enum
{
    A_INSTRUCTION,
    C_INSTRUCTION,
    L_INSTRUCTION
} InstructionType;

typedef enum{
    JMP_INSTRUCTION,
    COMP_INSTRUCTION
} SubInstructionType;
#endif