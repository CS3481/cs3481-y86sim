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

bool WritebackStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    W * wreg = (W *) pregs[WREG];
    uint64_t icode = wreg->geticode()->getOutput();
    
    if (icode == 0)
        return true;
    else
        return false;
}

void WritebackStage::doClockHigh(PipeReg ** pregs)
{
    RegisterFile * reg = RegisterFile::getInstance();
    W * wreg = (W *) pregs[WREG];
        
    bool err = false;
    reg->writeRegister(wreg->getvalE()->getOutput(), wreg->getdstE()->getOutput(), err);
}
