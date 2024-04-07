
# Assembler for Hack Operating System Written in C
```bash
This is a part of the project in NandToTetris.

The assembler contains a LR parser, Symbol table and Instruction generator. 
````
## How to build
```bash
Require GCC to compile 
gcc -O -o assembler ./Parser/Parser.c ./Symbol/Symbol.c ./Code/Code.c main.c

Or Using Makefile
make build
```

## Building a program
```bash
1.File with .asm and valid Hack Instruction set is required to build.
2.Max.asm contains valid Hack Instruction sets.
3.The assembler will output out.hack program which can be run on Hack's CPU Simulator.
```