#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "FetchStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "Memory.h"
#include "Tools.h"

/*
 * doClockLow:
 * Performs the Fetch stage combinational logic that is performed when
 * the clock edge is low.
 *
 * @param: pregs - array of the pipeline register sets (F, D, E, M, W instances)
 * @param: stages - array of stages (FetchStage, DecodeStage, ExecuteStage,
 *         MemoryStage, WritebackStage instances)
 */
bool FetchStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];
   M * mreg = (M *) pregs[MREG];
   W * wreg = (W *) pregs[WREG];
   uint64_t f_pc = 0, icode = 0, ifun = 0, valC = 0, valP = 0;
   uint64_t rA = RNONE, rB = RNONE, stat = SAOK;

   //code missing here to select the value of the PC
   //and fetch the instruction from memory
   //Fetching the instruction will allow the icode, ifun,
   //rA, rB, and valC to be set.
   //The lab assignment describes what methods need to be
   //written.
   f_pc = selectPC(freg, mreg, wreg, f_pc);
   
   Memory * mem = Memory::getInstance();
   bool error;
   uint8_t byte = mem->getByte(f_pc, error); //gets first byte
   ifun = Tools::getBits(byte, 0, 3);
   icode = Tools::getBits(byte, 4, 7);  
   bool checkNeedIds = needRegIds(icode);
   bool checkNeedValC = needValC(icode);
   valP = PCincrement(f_pc, checkNeedIds, checkNeedValC);
   uint64_t prdct = predictPC(icode, valC, valP);
    
   //for getRegIds
   byte = mem->getByte(f_pc + 1, error);
   if (checkNeedIds)
       getRegIds(rA, rB, byte);

   //for buildValC
   if (checkNeedValC)
   {
       for (int i = 2; i < 10; i++)
       {
           //IN PROGRESS
           byte  = mem->getByte(f_pc + i, error);
           buildValC(valC, byte);
       }
   }
    
   //The value passed to setInput below will need to be changed
   freg->getpredPC()->setInput(prdct);

   //provide the input values for the D register
   setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);
   return false;
}

uint64_t FetchStage::PCincrement(uint64_t f_pc, bool checkNeedIds, bool checkNeedValC)
{
    uint64_t size = f_pc;
    size++;

    if (checkNeedIds)
        size += 1;

    if (checkNeedValC)
        size += 8;

    return size; 
}

uint64_t FetchStage::selectPC(F * freg, M * mreg, W * wreg, uint64_t f_pc)
{
    if (mreg->geticode()->getOutput() == IJXX)
        f_pc = mreg->getvalA()->getOutput();
    else if (wreg->geticode()->getOutput() == IRET)
        f_pc = wreg->getvalM()->getOutput();
    else
        f_pc = freg->getpredPC()->getOutput();

    return f_pc;
}

bool FetchStage::needRegIds(uint64_t f_icode)
{
    return (f_icode == IRRMOVQ || f_icode == IOPQ || f_icode == IPUSHQ || f_icode == IPOPQ
            || f_icode == IIRMOVQ || f_icode == IRMMOVQ || f_icode == IMRMOVQ);
}

bool FetchStage::needValC(uint64_t f_icode)
{
    return (f_icode == IIRMOVQ || f_icode == IRMMOVQ || f_icode == IMRMOVQ || f_icode == IJXX
            || f_icode == ICALL);
}

uint64_t FetchStage::predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP)
{
    if (f_icode == IJXX || f_icode == ICALL)
        return f_valC;
    else
        return f_valP;
}

/* doClockHigh
 * applies the appropriate control signal to the F
 * and D register intances
 *
 * @param: pregs - array of the pipeline register (F, D, E, M, W instances)
 */
void FetchStage::doClockHigh(PipeReg ** pregs)
{
   F * freg = (F *) pregs[FREG];
   D * dreg = (D *) pregs[DREG];

   freg->getpredPC()->normal();
   dreg->getstat()->normal();
   dreg->geticode()->normal();
   dreg->getifun()->normal();
   dreg->getrA()->normal();
   dreg->getrB()->normal();
   dreg->getvalC()->normal();
   dreg->getvalP()->normal();
}

/* setDInput
 * provides the input to potentially be stored in the D register
 * during doClockHigh
 *
 * @param: dreg - pointer to the D register instance
 * @param: stat - value to be stored in the stat pipeline register within D
 * @param: icode - value to be stored in the icode pipeline register within D
 * @param: ifun - value to be stored in the ifun pipeline register within D
 * @param: rA - value to be stored in the rA pipeline register within D
 * @param: rB - value to be stored in the rB pipeline register within D
 * @param: valC - value to be stored in the valC pipeline register within D
 * @param: valP - value to be stored in the valP pipeline register within D
*/
void FetchStage::setDInput(D * dreg, uint64_t stat, uint64_t icode, 
                           uint64_t ifun, uint64_t rA, uint64_t rB,
                           uint64_t valC, uint64_t valP)
{
   dreg->getstat()->setInput(stat);
   dreg->geticode()->setInput(icode);
   dreg->getifun()->setInput(ifun);
   dreg->getrA()->setInput(rA);
   dreg->getrB()->setInput(rB);
   dreg->getvalC()->setInput(valC);
   dreg->getvalP()->setInput(valP);
}


void FetchStage::getRegIds(uint64_t & rA, uint64_t & rB, uint8_t byte)
{
    rA = RNONE; //F
    rB = Tools::getBits(byte, 0, 3);
}


void FetchStage::buildValC(uint64_t & valC, uint8_t byte)
{
    //how to add on each byte????
    valC = Tools::getBits(byte, 0, 3);
}
