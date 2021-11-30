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

FetchStage.o: Tools.h Memory.h Instructions.h RegisterFile.h PipeRegField.h PipeReg.h F.h D.h M.h W.h Stage.h FetchStage.h Status.h Debug.h

DecodeStage.o: Instructions.h PipeRegField.h PipeReg.h D.h E.h M.h W.h Stage.h DecodeStage.h ExecuteStage.h Status.h Debug.h RegisterFile.h

ExecuteStage.o: Tools.h ConditionCodes.h Instructions.h RegisterFile.h PipeRegField.h PipeReg.h E.h M.h Stage.h ExecuteStage.h Status.h Debug.h

MemoryStage.o: Memory.h Instructions.h RegisterFile.h PipeRegField.h PipeReg.h M.h W.h Stage.h MemoryStage.h Status.h Debug.h

WritebackStage.o: RegisterFile.h PipeRegField.h PipeReg.h W.h Stage.h WritebackStage.h Status.h Debug.h

Simulate.o: PipeRegField.h PipeReg.h F.h D.h E.h M.h W.h Stage.h FetchStage.h DecodeStage.h ExecuteStage.h MemoryStage.h WritebackStage.h Simulate.h Memory.h RegisterFile.h ConditionCodes.h

F.o: PipeRegField.h PipeReg.h F.h

D.o: PipeRegField.h PipeReg.h D.h Instructions.h RegisterFile.h Status.h

E.o: PipeRegField.h PipeReg.h E.h RegisterFile.h Instructions.h Status.h

M.o: PipeRegField.h PipeReg.h M.h RegisterFile.h Instructions.h Status.h

W.o: PipeRegField.h PipeReg.h W.h RegisterFile.h Instructions.h Status.h

PipeReg.o: PipeReg.h 

PipeRegField.o: PipeRegField.h 



clean:
	rm $(OBJ) yess

run:
	make clean
	make yess
	./run.sh

