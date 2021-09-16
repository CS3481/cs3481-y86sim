CC = g++
CFLAGS = -g -c -Wall -std=c++11 -O0
OBJ = lab4.o MemoryTester.o Memory.o Tools.o RegisterFile.o \
RegisterFileTester.o ConditionCodes.o ConditionCodesTester.o

.C.o:
	$(CC) $(CFLAGS) $< -o $@

lab4: $(OBJ) 

lab4.o: Memory.h RegisterFile.h MemoryTester.h RegisterFileTester.h ConditionCodes.h \
	ConditionCodesTester.h 

Tools.o: Tools.h 

Memory.o: Memory.h Tools.h

MemoryTester.o: MemoryTester.h Memory.h

RegisterFile.o: RegisterFile.h Tools.h

RegisterFileTester.o: RegisterFileTester.h RegisterFile.h

ConditionCodes.o: ConditionCodes.h Tools.h

ConditionCodesTester.o: ConditionCodesTester.h ConditionCodes.h

clean:
	rm $(OBJ) lab4

run:
	make clean
	make lab4
	./run.sh

