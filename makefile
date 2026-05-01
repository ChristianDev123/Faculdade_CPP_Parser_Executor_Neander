GCC = g++

all: run_assembly run_executor

run_executor:executor
	./executor.o output.nexe
run_assembly: assembly
	./assembler.o output.nexe
executor: clean
	$(GCC) executor.cpp -o executor.o
assembly: clean
	$(GCC) assembler.cpp -o assembler.o 
clean:
	rm -rf *.o