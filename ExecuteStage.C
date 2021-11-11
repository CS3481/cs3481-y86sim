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
#include "Instructions.h"

bool ExecuteStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    E * ereg = (E *) pregs[EREG];
    M * mreg = (M *) pregs[MREG];    
    uint64_t icode = ereg->geticode()->getOutput(), valE = ereg->getvalC()->getOutput(),
        valA = ereg->getvalA()->getOutput(), dstE = ereg->getdstE()->getOutput(),
        dstM = ereg->getdstM()->getOutput(), Cnd = 0, stat = SAOK, 
        ifun = ereg->getifun()->getOutput();

    uint64_t aluA;
    uint64_t aluB;
    uint64_t alufun;
    bool set_cc;

    setAluA(ereg, aluA, icode);
    setAluB(ereg, aluB, icode);
    setAluFun(alufun, icode, ifun);
    setCC(set_cc, icode);
    setDstE(ereg, dstE, icode, Cnd);
    
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

void ExecuteStage::setAluA(E * ereg, uint64_t & aluA, uint64_t e_icode)
{
    if (e_icode == IRRMOVQ || e_icode == IOPQ)
        aluA = ereg->getvalA()->getOutput();
    else if (e_icode == IIRMOVQ || e_icode == IRMMOVQ || e_icode == IMRMOVQ)
        aluA = ereg->getvalC()->getOutput();
    else if (e_icode == ICALL || e_icode == IPUSHQ)
        aluA = -8;
    else if (e_icode == IRET || e_icode == IPOPQ)
        aluA = 8;
    else
        aluA = 0;
}

void ExecuteStage::setAluB(E * ereg, uint64_t & aluB, uint64_t e_icode)
{
    if (e_icode == IRMMOVQ || e_icode == IMRMOVQ || e_icode == IOPQ
        || e_icode == ICALL || e_icode == IPUSHQ || e_icode == IRET
        || e_icode == IPOPQ)
    {
        aluB = ereg->getvalB()->getOutput();
    }
    else
        aluB = 0;
} 

void ExecuteStage::setAluFun(uint64_t & alufun, uint64_t e_icode, uint64_t ifun)
{
    if (e_icode == IOPQ)
        alufun = ifun;
    else 
        alufun = ADDQ;
}

void ExecuteStage::setCC(bool & set_cc, uint64_t e_icode)
{
    set_cc = (e_icode == IOPQ);
}

void ExecuteStage::setDstE(E * ereg, uint64_t & dstE, uint64_t e_icode, uint64_t e_Cnd)
{
    if (e_icode == IRRMOVQ && !e_Cnd) //change this
        dstE = RNONE;
    else
        dstE = ereg->getdstE()->getOutput(); //may be wrong dstE assigned
}
