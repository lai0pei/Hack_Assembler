#include "./Parser.h"
#include <string.h>
#define BUFFER 128
#define VAR_SIZE 10
#define SYM_SIZE 50
#define __NEXT                    \
    LOOKAHEAD = BUFF[NEXT_INDEX]; \
    NEXT_INDEX++;
#define __EMIT_ERROR(str)                                                                                     \
    fprintf(stderr, "%s at line number %d , position %d. Instruction : %s \n", str, TRACK, CHAR_COUNT, BUFF); \
    exit(0);

static FILE *FP;
char *FILE_NAME;
static char *BUFF;
unsigned int L_INSTRUCTION_LINES = 1;
static bool EOL = false;
static unsigned int TRACK = 0;
static unsigned int CHAR_COUNT = 0;
InstructionType TYPE;
SubInstructionType SUB_TYPE;
static const char *jmp[7] = {"JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP"};
static char LOOKAHEAD;
static unsigned int NEXT_INDEX = 0;
static char dest[VAR_SIZE] = {};
static char comp[VAR_SIZE] = {};
static char jump[VAR_SIZE] = {};
static char __symbol[SYM_SIZE] = {};

static void __a_instruction(void);
static void __l_instruction(void);
static void __c_instruction(void);
static void __comp(void);
static void __jmp(void);
static void __unary_op(void);
static void __compOp(void);
static void __destOp(void);
static bool __is_valid_alpha(char);

void TypeCheck(void)
{
    printf("%s\n", BUFF);
    switch (TYPE)
    {
    case A_INSTRUCTION:
        printf("A_INSTRUCTION\n");
        break;
    case C_INSTRUCTION:
        printf("C_INSTRUCTION\n");
        break;
    case L_INSTRUCTION:
        printf("L_INSTRUCTION\n");
        break;
    }
}

// multi passes require to reset to default count
void parser_init()
{
    EOL = false;
    TRACK = 0;
    CHAR_COUNT = 0;
    L_INSTRUCTION_LINES = 0;
}

void file_constructor()
{
    FP = fopen(FILE_NAME, "r");
    if (FP == NULL)
    {
        perror("");
        exit(0);
    }
    BUFF = (char *)malloc(sizeof(char) * BUFFER);
    if (BUFF == NULL)
    {
        perror("");
        exit(0);
    }
}

bool hasMoreLines(void)
{
    return !EOL;
}

int advance(void)
{
    if (FP == NULL)
    {
        fprintf(stderr, "Open file stream first");
        exit(0);
    }
    int tmp = 0;
    unsigned int buff_count = 0;
    bool is_comment = false;
    CHAR_COUNT = 0;
    NEXT_INDEX = 0;
    memset(BUFF, 0, sizeof(char) * BUFFER);
    while ((tmp = fgetc(FP)))
    {
        if (tmp == '\n')
        {
            TRACK++;
            break;
        }

        if (tmp == EOF)
        {
            EOL = true;
            break;
        }

        if (is_comment || isspace(tmp) > 0)
        {
            continue;
        }

        if (tmp == '/' && fgetc(FP) == '/')
        {
            is_comment = true;
            continue;
        }

        BUFF[buff_count] = tmp;
        buff_count++;
        CHAR_COUNT++;
    }

    if (is_comment)
        return 0;

    return buff_count;
}

InstructionType instructionType(void)
{
    return TYPE;
}

void parse_dest(char *c)
{
    if (SUB_TYPE == COMP_INSTRUCTION)
    {
        memset(dest, '\0', VAR_SIZE);
        char i = 0;
        while (BUFF[i] != '=')
        {
            dest[i] = BUFF[i];
            i++;
        }
        strcpy(c, dest);
    }
}

void parse_comp(char *c)
{
    if (TYPE == C_INSTRUCTION)
    {
        memset(comp, '\0', VAR_SIZE);
        char i = 0;
        switch (SUB_TYPE)
        {
        case COMP_INSTRUCTION:

            while (BUFF[i++] != '=')
                ;
            char i2 = 0;
            while (BUFF[i] != '\0')
            {
                comp[i2] = BUFF[i];
                i2++;
                i++;
            }

            break;
        case JMP_INSTRUCTION:
            while (BUFF[i] != ';')
            {
                comp[i] = BUFF[i];
                i++;
            }
            break;
        }
        strcpy(c, comp);
    }
}

void parse_jump(char *c)
{
    if (SUB_TYPE == JMP_INSTRUCTION)
    {
        memset(jump, '\0', VAR_SIZE);
        char i = 0;
        while (BUFF[i++] != ';')
            ;
        char i2 = 0;
        while (BUFF[i] != '\0')
        {
            jump[i2] = BUFF[i];
            i2++;
            i++;
        }
        strcpy(c, jump);
    }
}

void parse_symbol(char *c)
{
    char index = 0;
    char i = 0;
    memset(__symbol, '\0', SYM_SIZE);
    switch (BUFF[index++])
    {
    case '@':
        while (BUFF[index] != '\0')
        {
            __symbol[i] = BUFF[index];
            i++;
            index++;
        }
        break;
    case '(':
        while (BUFF[index] != ')')
        {
            __symbol[i] = BUFF[index];
            i++;
            index++;
        }
        break;
    }
    strcpy(c, __symbol);
}

void expression()
{
    __NEXT
    switch (LOOKAHEAD)
    {
    case '@':
        TYPE = A_INSTRUCTION;
        L_INSTRUCTION_LINES++;
        __a_instruction();
        break;
    case '(':
        TYPE = L_INSTRUCTION;
        __l_instruction();
        break;
    default:
        TYPE = C_INSTRUCTION;
        L_INSTRUCTION_LINES++;
        __c_instruction();
    }
}

void __c_instruction()
{
    __destOp();

    switch (LOOKAHEAD)
    {
    case ';':
        SUB_TYPE = JMP_INSTRUCTION;
        __jmp();
        break;
    case '=':
        SUB_TYPE = COMP_INSTRUCTION;
        __comp();
        break;
    default:
        __EMIT_ERROR("Unsupported instruction instruction")
    }
}

void __destOp()
{
    bool s = LOOKAHEAD == 'A' || LOOKAHEAD == 'D' || LOOKAHEAD == 'M' || LOOKAHEAD == '0';
    char t = 0;
    char dest_local[5] = {};
    bool is_digit = false;
    if (!s)
    {
        __EMIT_ERROR("Variables should start with A,D,M or 0")
    }

    dest_local[t++] = LOOKAHEAD;
    __NEXT

    char tc = 0;
    while (LOOKAHEAD != ';' && LOOKAHEAD != '=')
    {

        if (isdigit(LOOKAHEAD))
            is_digit = true;

        tc = 0;
        while (dest_local[tc] != '\0')
        {
            if (LOOKAHEAD == dest_local[tc])
            {
                __EMIT_ERROR("It is neither Assignment nor Jump instruction")
            }

            tc++;
        }
        dest_local[t] = LOOKAHEAD;
        t++;
        __NEXT
    }

    if (t > 1 && is_digit)
    {
        __EMIT_ERROR("Unsupported instruction instruction at left side of operator")
    }
}

void __comp()
{
    __NEXT
    if (LOOKAHEAD == '-' || LOOKAHEAD == '!')
    {
        __unary_op();
    }
    else if (LOOKAHEAD == 'A' || LOOKAHEAD == 'D' || LOOKAHEAD == 'M')
    {
        __compOp();
    }
    else if (LOOKAHEAD != '0' && LOOKAHEAD != '1')
    {
        __EMIT_ERROR("Only support numeric 0 or 1 or Sign - or !")
    }
    __NEXT
    if (LOOKAHEAD != '\0')
    {
        __EMIT_ERROR("Unsupported instruction")
    }
}

void __unary_op()
{
    __NEXT
    bool s = LOOKAHEAD == 'M' || LOOKAHEAD == 'A' || LOOKAHEAD == 'D' || LOOKAHEAD == '1';
    if (!s)
    {
        __EMIT_ERROR("Variables A,M,D or numeric 1,0")
    }
    __NEXT
    if (LOOKAHEAD != '\0')
    {
        __EMIT_ERROR("Unsupported instruction Unary Operator")
    }
}

void __compOp()
{
    bool c1 = LOOKAHEAD == 'M' || LOOKAHEAD == 'A' || LOOKAHEAD == 'D' || LOOKAHEAD == '1';
    if (!c1)
    {
        __EMIT_ERROR("Unsupported instruction Compute variable on left side of operator")
    }
    __NEXT
    if (LOOKAHEAD != '\0')
    {
        bool op = LOOKAHEAD == '+' || LOOKAHEAD == '-' || LOOKAHEAD == '|' || LOOKAHEAD == '&';
        if (!op)
        {
            __EMIT_ERROR("Supported Compute Operator +,-,|,&")
        }

        __NEXT
        if (LOOKAHEAD != '\0')
        {
            bool c2 = LOOKAHEAD == 'M' || LOOKAHEAD == 'A' || LOOKAHEAD == 'D' || LOOKAHEAD == '1';
            if (!c2)
            {
                __EMIT_ERROR("Supported Compute variable on right side of operator M,A,D,1")
            }
            __NEXT
            if (LOOKAHEAD != '\0')
            {
                __EMIT_ERROR("Invalid Instruction")
            }
        }
        else
        {
            __EMIT_ERROR("Incomplete instruction")
        }
    }
}

void __jmp()
{
    char c[3] = {BUFF[NEXT_INDEX], BUFF[NEXT_INDEX + 1], BUFF[NEXT_INDEX + 2]};
    char i = 0;
    char t = 0;
    bool f = false;
    while (jmp[i] != NULL)
    {
        t = strncmp(c, jmp[i], 3);
        if (t == 0)
        {
            f = true;
            break;
        }
        i++;
    }
    if (!f)
    {
        __EMIT_ERROR("Unsupported instruction Jump")
    }
}

void __a_instruction()
{
    __NEXT
    bool is_first_digit = false;
    bool s = false;

    if (LOOKAHEAD == '\0')
    {
        __EMIT_ERROR("Empty Symbol")
    }

    if (isdigit(LOOKAHEAD))
    {
        is_first_digit = true;
        __NEXT
    }

    while (LOOKAHEAD != '\0')
    {
        s = __is_valid_alpha(LOOKAHEAD);

        if (is_first_digit && s)
        {
            __EMIT_ERROR("A symbol can be any sequence of letters, digits, underscore (_), dot (.), dollar sign ($), and colon (:) that does not begin with a digit.")
        }

        if (!s && !isdigit(LOOKAHEAD))
        {
            __EMIT_ERROR("A symbol can be any sequence of letters, digits, underscore (_), dot (.), dollar sign ($), and colon (:) that does not begin with a digit.")
        }
        __NEXT
    }
}

void __l_instruction()
{
    __NEXT
    if (LOOKAHEAD == ')' || isdigit(LOOKAHEAD))
    {
        __EMIT_ERROR("Name this jump location")
    }

    while (LOOKAHEAD != ')')
    {
        if (!__is_valid_alpha(LOOKAHEAD) && !isdigit(LOOKAHEAD))
        {
            __EMIT_ERROR("A symbol can be any sequence of letters, digits, underscore (_), dot (.), dollar sign ($), and colon (:) that does not begin with a digit.")
        }
        __NEXT
    }
}

bool __is_valid_alpha(char t)
{
    if (isalpha(t) || t == '_' || t == '$' || t == '.' || t == ':')
        return true;

    return false;
}

void clean(void)
{
    free(BUFF);
    fclose(FP);
}
