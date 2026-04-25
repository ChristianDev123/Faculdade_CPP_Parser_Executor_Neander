GCC = g++
all: assembly
	./assembler.o
assembly: clean
	$(GCC) assembler.cpp -o assembler.o 
clean:
	rm -rf *.o