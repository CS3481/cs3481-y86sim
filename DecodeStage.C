#include <string>
#include <cstdint>
#include "PipeRegField.h"
#include "PipeReg.h"
#include "D.h"
#include "Stage.h"
#include "DecodeStage.h"
#include "Status.h"
#include "Debug.h"

bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    return false;
}

void DecodeStage::doClockHigh(PipeReg ** pregs)
{

}
