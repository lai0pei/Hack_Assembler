files = ./Parser/Parser.c ./Symbol/Symbol.c ./Code/Code.c main.c
build : 
		gcc -O -o assembler $(files)
clean : 
		rm ./assembler
