all: test

test: test.asm
	 nasm -o test.elf64 -f elf64 test.asm

test_debug:
	nasm -e test.asm     

clean:
	 rm test.elf64
