#include <string>
#include <cstdint>
#include "PipeRegField.h"
#include "PipeReg.h"
#include "W.h"
#include "Stage.h"
#include "WritebackStage.h"
#include "Status.h"
#include "Debug.h"

bool WritebackStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    return true;
}

void WritebackStage::doClockHigh(PipeReg ** pregs)
{

}
