#include <string>
#include <cstdint>
#include "PipeRegField.h"
#include "PipeReg.h"
#include "W.h"
#include "Stage.h"
#include "WritebackStage.h"
#include "Status.h"
#include "Debug.h"
#include "RegisterFile.h"

/*
 * doClockLow
 * low clock cycle of writeback stage
 *
 * @param pregs is something
 * @param stages is something else
 *
 * @return false if no errors
 */
bool WritebackStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    W * wreg = (W *) pregs[WREG];
    //uint64_t icode = wreg->geticode()->getOutput();
    uint64_t stat = wreg->getstat()->getOutput();
      
    //lab11 pt II
    if (stat != SAOK)
    {
        return true;
    }
    else
    {
        return false;
    }
    
    /*
    if (icode == 0)
        return true;
    else
        return false;
    */
}

/*
 * doClockHigh
 * high clock cycle for writeback stage
 *
 * @param pregs is something
 */
void WritebackStage::doClockHigh(PipeReg ** pregs)
{
    RegisterFile * reg = RegisterFile::getInstance();
    W * wreg = (W *) pregs[WREG];
        
    bool err = false;
    reg->writeRegister(wreg->getvalE()->getOutput(), wreg->getdstE()->getOutput(), err);
    reg->writeRegister(wreg->getvalM()->getOutput(), wreg->getdstM()->getOutput(), err);
}
