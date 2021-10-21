CC = g++
CFLAGS = -g -c -Wall -std=c++11 -O0
OBJ = Memory.o Tools.o RegisterFile.o ConditionCodes.o Loader.o \
 FetchStage.o DecodeStage.o ExecuteStage.o MemoryStage.o \
 WritebackStage.o Simulate.o \
 F.o D.o E.o M.o W.o \
 PipeReg.o PipeRegField.o

.C.o:
	$(CC) $(CFLAGS) $< -o $@

yess: $(OBJ) 

yess.o: Memory.h RegisterFile.h  ConditionCodes.h Loader.h

Tools.o: Tools.h 

Memory.o: Memory.h Tools.h

RegisterFile.o: RegisterFile.h Tools.h

ConditionCodes.o: ConditionCodes.h Tools.h

Loader.o: Loader.h Memory.h

FetchStage.o:

DecodeStage.o:

ExecuteStage.o:

MemoryStage.o:

WritebackStage.o:

Simulate.o:

F.o:

D.o:

E.o:

M.o:

W.o:

PipeReg.o:

PipeRegField.o:



clean:
	rm $(OBJ) yess

run:
	make clean
	make yess
	./yess

