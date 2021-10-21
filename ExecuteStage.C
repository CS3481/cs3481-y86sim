#include <string>
#include <cstdint>
#include "PipeRegField.h"
#include "PipeReg.h"
#include "M.h"
#include "W.h"
#include "E.h"
#include "Stage.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"

bool ExecuteStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    return false;
}

void ExecuteStage::doClockHigh(PipeReg ** pregs)
{

}
