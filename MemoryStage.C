#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "Instructions.h"
#include "Status.h"
#include "Debug.h"
#include "Memory.h"

/*
 * doClockLow
 * Low clock cycle in memory stage
 *
 * @param pregs is ptr to pipe reg
 * @param stages is something
 *
 * @return bool true if errors
 */
bool MemoryStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    M * mreg = (M *) pregs[MREG];
    W * wreg = (W *) pregs[WREG];
    Memory * mem = Memory::getInstance();

    uint64_t icode = mreg->geticode()->getOutput(), valE = mreg->getvalE()->getOutput(), 
             dstE = mreg->getdstE()->getOutput(), dstM = mreg->getdstM()->getOutput();
    uint64_t valA = mreg->getvalA()->getOutput();
    bool mem_error = false;
    valM = 0;
    uint32_t address = addr(icode, valE, valA);
    stat = mreg->getstat()->getOutput();

    if (memRead(icode))
        valM = mem->getLong(address, mem_error); 

    if (memWrite(icode))
        mem->putLong(valA, address, mem_error);    
    
    //lab11 pt II
    if (mem_error)
    {
        stat = SADR;
    }

    setWInput(wreg, stat, icode, valE, valM, dstE, dstM);

    return false;
}

/*
 * doClockHigh
 * high clock cycle for mem stage
 *
 * @param pregs is something pointing at some stuff
 */
void MemoryStage::doClockHigh(PipeReg ** pregs)
{
    W * wreg = (W *) pregs[WREG];

    wreg->getstat()->normal();
    wreg->geticode()->normal();
    wreg->getvalE()->normal();
    wreg->getvalM()->normal();
    wreg->getdstE()->normal();
    wreg->getdstM()->normal();
}

/*
 * setWInput
 * sets inputs for next stage
 *
 * @param wreg is ptr to writeback stage
 * @param stat val for stat
 * @param icode val for icode
 * @param valE val for valE
 * @param valM val for valM
 * @param dstE val for dstE
 * @param dstM val for dstM
 */
void MemoryStage::setWInput(W * wreg, uint64_t stat, uint64_t icode, uint64_t valE, uint64_t valM,
                                uint64_t dstE, uint64_t dstM)
{
    wreg->getstat()->setInput(stat);  
    wreg->geticode()->setInput(icode);
    wreg->getvalE()->setInput(valE);
    wreg->getvalM()->setInput(valM);
    wreg->getdstE()->setInput(dstE);
    wreg->getdstM()->setInput(dstM);      
}

/*
 * addr
 * gets address for valM
 *
 * @param icode is icode
 * @param valE is that
 * @param valA is that
 *
 * @return address
 */
uint32_t MemoryStage::addr(uint64_t icode, uint64_t valE, uint64_t valA)
{
    if (icode == IRMMOVQ || icode == IPUSHQ || icode == ICALL || icode == IMRMOVQ)
         return valE;
    else if (icode == IPOPQ || icode == IRET)
        return valA;
    else
        return 0;
}

/*
 * memRead
 * true if read memory
 *
 * @param icode is icode
 * 
 * @return boolean if true
 */
bool MemoryStage::memRead(uint64_t icode)
{
    return (icode == IMRMOVQ || icode == IPOPQ || icode == IRET);
}

/*
 * memWrite
 * writes memory
 * 
 * @param icode is that
 *
 * @return if true
 */
bool MemoryStage::memWrite(uint64_t icode)
{
    return (icode == IRMMOVQ || icode == IPUSHQ || icode == ICALL);
}

/*
 * getValM
 * returns valM
 *
 * @return valM
 */
uint64_t MemoryStage::getValM()
{
    return valM;
}

/*
 * getStat
 * returns stat
 *
 * @return stat
 */
uint64_t MemoryStage::getStat()
{
    return stat;
}
