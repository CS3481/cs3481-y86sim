#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "E.h"
#include "F.h"
#include "D.h"
#include "M.h"
#include "W.h"
#include "Memory.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "ExecuteStage.h"
#include "DecodeStage.h"
#include "FetchStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
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
    //E * ereg = (E *) pregs[EREG];
    M * mreg = (M *) pregs[MREG];
    W * wreg = (W *) pregs[WREG];
    //DecodeStage * dObj = (DecodeStage *) stages[DSTAGE];
    //ExecuteStage * eObj = (ExecuteStage *) stages[ESTAGE];
    uint64_t f_pc = 0, icode = 0, ifun = 0, valC = 0, valP = 0;
    uint64_t rA = RNONE, rB = RNONE, stat = SAOK;
    F_stall = false;
    D_stall = false;
    f_pc = selectPC(freg, mreg, wreg, f_pc);

    Memory * mem = Memory::getInstance();
    bool mem_error;
    uint8_t byte = mem->getByte(f_pc, mem_error); //gets first byte   
    ifun = Tools::getBits(byte, 0, 3);
    icode = Tools::getBits(byte, 4, 7); 
    
    //lab11
    if (mem_error)
    {
        icode = INOP;
        ifun = FNONE;
    }

    bool checkNeedIds = needRegIds(icode);
    bool checkNeedValC = needValC(icode);
    
    valP = PCincrement(f_pc, checkNeedIds, checkNeedValC);
    
    //for getRegIds & build ValC
    if (checkNeedIds)
    {
        getRegIds(rA, rB, f_pc);
        if (checkNeedValC)
        {
            valC = buildValC(f_pc, checkNeedIds);
        }
        else
        {
            valC = 0;
        }
    }
    else
    {
        if (checkNeedValC)
        {
            valC = buildValC(f_pc, checkNeedIds);
        }
        else
        {
            valC = 0;
        }
    }
    
    uint64_t prdct = predictPC(icode, valC, valP);   
    stat = setStat(icode, mem_error);

    freg->getpredPC()->setInput(prdct);
    
    calculateControlSignals(pregs, stages);

    setDInput(dreg, stat, icode, ifun, rA, rB, valC, valP);
    return false;
}

/*
 * bubbleD
 * bubbles D reg
 *
 * @param pregs is pregs idk
 */
void FetchStage::bubbleD(PipeReg ** pregs)
{
    D * dreg = (D *) pregs[DREG];

    dreg->getstat()->bubble(SAOK);
    dreg->geticode()->bubble(INOP);
    dreg->getifun()->bubble();
    dreg->getrA()->bubble(RNONE);
    dreg->getrB()->bubble(RNONE);
    dreg->getvalC()->bubble();
    dreg->getvalP()->bubble();
}

/*
 * calcBubble
 * calculates bubble
 *
 * @param eObj is an ExecuteStage object
 * @param ereg E obj pointer
 */
bool FetchStage::calcBubble(uint64_t D_icode, uint64_t M_icode, uint64_t E_icode, uint64_t e_Cnd, uint64_t d_srcA, uint64_t d_srcB, uint64_t E_dstM)
{
    return (E_icode == IJXX && !e_Cnd) ||
        (!((E_icode == IMRMOVQ || E_icode == IPOPQ) && 
        (E_dstM == d_srcA || E_dstM == d_srcB)) &&
        (D_icode == IRET || E_icode == IRET || M_icode == IRET));
}

/*
 * calcFStall
 * calculates F_stall
 *
 * @param dObj for getting private data
 * @param ereg for getting vals
 * @param icode is icode
 */
void FetchStage::calcFStall(uint64_t D_icode, uint64_t M_icode, uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB)
{
     if (((E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == d_srcA || E_dstM == d_srcB)) ||
        (D_icode == IRET || E_icode == IRET || M_icode == IRET))
     {
        F_stall = true;
     }
     else
     {
        F_stall = false;
     }
}

/*
 * calcDStall
 * calculates D_stall
 *
 * @param dObj for getting private data
 * @param ereg for getting vals
 * @param icode is icode
 */
void FetchStage::calcDStall(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB)
{
    if ((E_icode == IMRMOVQ || E_icode == IPOPQ) && (E_dstM == d_srcA || E_dstM == d_srcB))
    {
        D_stall = true;
    }
    else
    {
        D_stall = false;
    }
}

/*
 * calculateControlSignals
 * calculates control signals by calling calcFStall and calcDStall
 */
void FetchStage::calculateControlSignals(PipeReg ** pregs, Stage ** stages)
{
    ExecuteStage * es = (ExecuteStage *) stages[ESTAGE];
    DecodeStage * ds = (DecodeStage *) stages[DSTAGE];
    E * ereg = (E *) pregs[EREG];
    M * mreg = (M *) pregs[MREG];
    D * dreg = (D *) pregs[DREG];
    
    uint64_t E_icode = ereg->geticode()->getOutput();
    uint64_t M_icode = mreg->geticode()->getOutput();
    uint64_t D_icode = dreg->geticode()->getOutput();
    uint64_t E_dstM = ereg->getdstM()->getOutput();
    uint64_t d_srcA = ds->getsrcA();
    uint64_t d_srcB = ds->getsrcB();
    uint64_t e_Cnd = es->getCnd();

    calcFStall(D_icode, M_icode, E_icode, E_dstM, d_srcA, d_srcB);
    calcDStall(E_icode, E_dstM, d_srcA, d_srcB);
    D_bubble = calcBubble(D_icode, M_icode, E_icode, e_Cnd, d_srcA, d_srcB, E_dstM);
}

/*
 * instrValid
 * returns true if icode if valid
 *
 * @param icode is that
 *
 * @return boolean true if icode is valid
 */
bool FetchStage::instrValid(uint64_t icode)
{
    if (icode == INOP || icode == IHALT || icode == IRRMOVQ || icode == IIRMOVQ
            || icode == IMRMOVQ || icode == IRMMOVQ || icode == IOPQ || icode == IJXX || icode == ICALL
            || icode == IRET || icode == IPUSHQ || icode == IPOPQ)
        return true;
    else
        return false;
}


/*
 * setStat
 * sets the val for stat
 *
 * @param icode is that
 * @param mem_error is error in mem from doClockLow
 *
 * @return val of stat
 */
uint64_t FetchStage::setStat(uint64_t icode, bool mem_error)
{
    if (mem_error)
    {
        return SADR;
    }
    
    if (!instrValid(icode))
    {
        return SINS;
    }
    
    if (icode == IHALT)
    {
        return SHLT;
    }
    
    return SAOK;
}


/*
 * PCincrement
 * increments the PC
 *
 * @param f_pc is something
 * @param checkNeedIds is bool for that method
 * @param chckNeedValC is bool for that method
 *
 * @return new PC
 */
uint64_t FetchStage::PCincrement(uint64_t f_pc, bool checkNeedIds, bool checkNeedValC)
{
    if (checkNeedValC)
    {
        if (checkNeedIds)
        {
            return f_pc + 10;
        }
        
        return f_pc + 9;
    }
    else
    {
        if (checkNeedIds)
        {
            return f_pc + 2;
        }
        return f_pc + 1;
    }
}

/*
 * selectPC
 * gets PC
 *
 * @param freg is ptr to F class
 * @param mreg is ptr to M class
 * @param wreg is ptr to W class
 * @param f_pc is that
 *
 * @return the f_pc
 */
uint64_t FetchStage::selectPC(F * freg, M * mreg, W * wreg, uint64_t f_pc)
{
    if (mreg->geticode()->getOutput() == IJXX && !mreg->getCnd()->getOutput())
        f_pc = mreg->getvalA()->getOutput();
    else if (wreg->geticode()->getOutput() == IRET)
        f_pc = wreg->getvalM()->getOutput();
    else
        f_pc = freg->getpredPC()->getOutput();

    return f_pc;
}

/*
 * needRegIds
 * checks if need regIds
 *
 * @param f_icode is val of icode in fetch stage
 *
 * @return boolean if regids are needed
 */
bool FetchStage::needRegIds(uint64_t f_icode)
{
    return (f_icode == IRRMOVQ || f_icode == IOPQ || f_icode == IPUSHQ || f_icode == IPOPQ
            || f_icode == IIRMOVQ || f_icode == IRMMOVQ || f_icode == IMRMOVQ);
}

/*
 * needValC
 * checks if valC is needed
 *
 * @param f_icode is val of f_icode
 *
 * @return bool if needValC
 */
bool FetchStage::needValC(uint64_t f_icode)
{
    return (f_icode == IIRMOVQ || f_icode == IRMMOVQ || f_icode == IMRMOVQ || f_icode == IJXX
            || f_icode == ICALL);
}

/*
 * predictPC
 * predicts the pc
 *
 * @param f_icode is val of icode
 * @param f_valC is val of valC
 * @param f_valP is val of valP
 *
 * @return the predicted pc
 */
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
   //D * dreg = (D *) pregs[DREG];
 
   if (!F_stall)
      freg->getpredPC()->normal();
   
    if (D_bubble)
        bubbleD(pregs);

   if (!D_stall)
       normalD(pregs);
}

/*
 * normalD
 * does normal things
 *
 * @param pregs is a parameter
 */
void FetchStage::normalD(PipeReg ** pregs)
{
    D * dreg = (D *) pregs[DREG];

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

/*
 * getRegIds
 * gets vals of rA and rB
 *
 * @param rA ref to rA
 * @param rB ref to rB
 * @param f_pc was the byte for rA and rB
 */
void FetchStage::getRegIds(uint64_t & rA, uint64_t & rB, uint64_t f_pc)
{
    Memory * memObj = Memory::getInstance();
    bool err = false;

    rA = memObj->getByte(f_pc+1, err);
    rA = rA >> 4;

    rB = memObj->getByte(f_pc+1, err);
    rB = rB & 0x0F;
}

/*
 * buildValC
 * builds val C
 *
 * @param f_pc was ref to valC
 * @param needRegIds was array of bits to become valC
 * 
 * @return valc 
 */
uint64_t FetchStage::buildValC(uint64_t f_pc, bool needRegIds)
{
    Memory * memObj = Memory::getInstance();
    uint64_t word = 0;
    bool err;

    if (needRegIds)
    {
        for (unsigned int i = 9; i >= 2; i--)
        {
            word += memObj->getByte(f_pc + i, err);
            if (i != 2)
            {
                word = word << 8;
            }
        }
    }
    else
    {
        for (unsigned int i = 8; i >= 1; i--)
        {
            word += memObj->getByte(f_pc + i, err);
            if (i != 1)
            {
                word = word << 8;
            }
        }
    }

    return word;
}

