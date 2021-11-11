#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "E.h"
#include "M.h"
#include "Stage.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"

bool ExecuteStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    E * ereg = (E *) pregs[EREG];
    M * mreg = (M *) pregs[MREG];    
    uint64_t icode = ereg->geticode()->getOutput(), valE = ereg->getvalC()->getOutput(),
        valA = ereg->getvalA()->getOutput(), dstE = ereg->getdstE()->getOutput(),
        dstM = ereg->getdstM()->getOutput(), Cnd = 0, stat = SAOK;

    setMInput(mreg, stat, icode, Cnd, valE, valA, dstE, dstM);

    return false;

}

void ExecuteStage::doClockHigh(PipeReg ** pregs)
{
    M * mreg = (M *) pregs[MREG];

    mreg->getstat()->normal();
    mreg->geticode()->normal();
    mreg->getCnd()->normal();
    mreg->getvalE()->normal();
    mreg->getvalA()->normal();
    mreg->getdstE()->normal();
    mreg->getdstM()->normal();
}

void ExecuteStage::setMInput(M * mreg, uint64_t stat, uint64_t icode, uint64_t Cnd, 
                                uint64_t valE, uint64_t valA, uint64_t dstE, uint64_t dstM)
{
    mreg->getstat()->setInput(stat);
    mreg->geticode()->setInput(icode);
    mreg->getCnd()->setInput(Cnd);
    mreg->getvalE()->setInput(valE);
    mreg->getvalA()->setInput(valA);
    mreg->getdstE()->setInput(dstE);
    mreg->getdstM()->setInput(dstM);
}
