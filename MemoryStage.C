#include <string>
#include <cstdint>
#include "PipeRegField.h"
#include "PipeReg.h"
#include "M.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "Status.h"
#include "Debug.h"

bool MemoryStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    return false;
}

void MemoryStage::doClockHigh(PipeReg ** pregs)
{

}
