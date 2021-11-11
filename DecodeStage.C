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
#include "Instructions.h"

bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    D * dreg = (D *) pregs[DREG];
    E * ereg = (E *) pregs[EREG];
    uint64_t icode = dreg->geticode()->getOutput(), ifun = dreg->getifun()->getOutput(), 
                valC = dreg->getvalC()->getOutput(), dstE = RNONE, dstM = RNONE, srcA = RNONE, 
                srcB = RNONE, stat = SAOK;  

    setEInput(ereg, stat, icode, ifun, dstE, dstM, valC, 0, srcA, srcB);
    setSrcA(dreg, srcA, icode);
    setSrcB(dreg, srcB, icode); 
    setDstE(dreg, dstE, icode);
    setDstM(dreg, dstM, icode);
    uint64_t tempValA = 0;
    uint64_t tempValB = 0;
    setValA(tempValA, srcA);
    setValB(tempValB, srcB);
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

void DecodeStage::setSrcA(D * dreg, uint64_t & srcA, uint64_t d_icode)
{
    if (d_icode == IRRMOVQ || d_icode == IRMMOVQ || d_icode == IOPQ
        || d_icode == IPUSHQ)
    {
        srcA = dreg->getrA()->getOutput();
    }
    else if (d_icode == IPOPQ || d_icode == IRET)
        srcA = RSP;
    else
        srcA = RNONE;
}

void DecodeStage::setSrcB(D * dreg, uint64_t & srcB, uint64_t d_icode) 
{
    if (d_icode == IOPQ || d_icode == IRMMOVQ || d_icode == IMRMOVQ)
        srcB = dreg->getrB()->getOutput(); 
    else if (d_icode == IPUSHQ || d_icode == IPOPQ || d_icode == ICALL
        || d_icode == IRET)
    {
        srcB = RSP;
    }
    else
        srcB = RNONE;
}

void DecodeStage::setDstE(D * dreg, uint64_t & dstE, uint64_t d_icode)
{
    if (d_icode == IRRMOVQ || d_icode == IIRMOVQ || d_icode == IOPQ)
        dstE = dreg->getrB()->getOutput();
    else if (d_icode == IPUSHQ || d_icode == IPOPQ || d_icode == ICALL
        || d_icode == IRET)
    {
        dstE = RSP;
    }
    else
        dstE = RNONE;
}

void DecodeStage::setDstM(D * dreg, uint64_t & dstM, uint64_t d_icode)
{
    if (d_icode == IMRMOVQ || d_icode == IPOPQ)
        dstM = dreg->getrA()->getOutput();
    else
        dstM = RNONE;
}

void DecodeStage::setValA(uint64_t & valA, uint64_t d_srcA)
{
    RegisterFile * reg = RegisterFile::getInstance();
    bool err = false;
    valA = reg->readRegister(d_srcA, err);
}

void DecodeStage::setValB(uint64_t & valB, uint64_t d_srcB)
{
    RegisterFile * reg = RegisterFile::getInstance();
    bool err = false;
    valB = reg->readRegister(d_srcB, err);
}

