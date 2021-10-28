#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "D.h"
#include "E.h"
#include "Stage.h"
#include "DecodeStage.h"
#include "Status.h"
#include "Debug.h"

bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    D * dreg = (D *) pregs[FREG];
    E * ereg = (E *) pregs[EREG];
    uint64_t icode = 0, ifun = 0, valC = 0, valP = 0, dstE = RNONE,
                    dstM = RNONE, srcA = RNONE, srcB = RNONE, stat = SAOK;  

    setEInput(ereg, stat, icode, ifun, dstE, dstM, valC, valP, srcA, srcB);
    return false;
}

void DecodeStage::doClockHigh(PipeReg ** pregs)
{
     //D * dreg = (D *) pregs[FREG];
     E * ereg = (E *) pregs[EREG];

    ereg->getstat()->normal();
    ereg->geticode()->normal();
    ereg->getifun()->normal();
    ereg->getdstE()->normal();
    ereg->getdstM()->normal();
    ereg->getvalC()->normal();
    ereg->getvalA()->normal();
    ereg->getsrcA()->normal();
    ereg->getsrcB()->normal();

}

void DecodeStage::setEInput(E * ereg, uint64_t stat, uint64_t icode,
                        uint64_t ifun, uint64_t dstE, uint64_t dstM,
                        uint64_t valC, uint64_t valA, uint64_t srcA,
                        uint64_t srcB)
{
    ereg->getstat()->setInput(stat);
    ereg->geticode()->setInput(icode);
    ereg->getifun()->setInput(ifun);
    ereg->getdstE()->setInput(dstE);
    ereg->getdstM()->setInput(dstM);
    ereg->getvalC()->setInput(valC);
    ereg->getvalA()->setInput(valA);
    ereg->getsrcA()->setInput(srcA);
    ereg->getsrcB()->setInput(srcB);
}
