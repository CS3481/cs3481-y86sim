#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "D.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "ExecuteStage.h"
#include "DecodeStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"

/*
 * doClockLow
 * Low clock cycle for the decode stage
 *
 * @param pregs is pointer to Pipe Register class
 * @param stages is pointer to all stages
 *
 * @return boolean false if no errors
 */
bool DecodeStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    D * dreg = (D *) pregs[DREG];
    E * ereg = (E *) pregs[EREG];
    M * mreg = (M *) pregs[MREG];
    W * wreg = (W *) pregs[WREG];
    ExecuteStage * eObj = (ExecuteStage *) stages[ESTAGE];
    MemoryStage * mObj = (MemoryStage *) stages[MSTAGE];

    uint64_t icode = dreg->geticode()->getOutput(), ifun = dreg->getifun()->getOutput(), 
                valC = dreg->getvalC()->getOutput(), dstE = RNONE, dstM = RNONE, 
                stat = dreg->getstat()->getOutput();  
    
    uint64_t valA, valB, valP;
    srcB = RNONE;
    srcA = RNONE;
    valP = dreg->getvalP()->getOutput();
    
    setSrcA(dreg, srcA, icode);
    setSrcB(dreg, srcB, icode); 
    setDstE(dreg, dstE, icode);
    setDstM(dreg, dstM, icode);
    setValA(valA, srcA, mreg, wreg, eObj, mObj, icode, valP);
    setValB(valB, srcB, mreg, wreg, eObj, mObj);
    
    
    uint64_t E_dstM = ereg->getdstM()->getOutput();
    uint64_t E_icode = ereg->geticode()->getOutput();
    uint64_t E_Cnd = eObj->getCnd();
    bubble = calculateControlSignals(E_icode, E_dstM, srcA, srcB, E_Cnd); 
    
    setEInput(ereg, stat, icode, ifun, dstE, dstM, valC, valA, valB, srcA, srcB);
    return false;
}

/*
 * doClockHigh
 * High clock cycle for decode stage
 *
 * @param pregs is ptr to pipe reg class
 */
void DecodeStage::doClockHigh(PipeReg ** pregs)
{
    E * ereg = (E *) pregs[EREG];
    
    if (bubble)
    {
        bubbleE(ereg);
    }
    else
    {
        normalE(ereg);
    }
}

/*
 * bubbleE
 * bubble for that
 *
 * @param ereg is cool
 */
void DecodeStage::bubbleE(E * ereg)
{
    ereg->getstat()->bubble(SAOK);
    ereg->geticode()->bubble(INOP);
    ereg->getifun()->bubble();
    ereg->getvalC()->bubble();
    ereg->getvalA()->bubble();
    ereg->getvalB()->bubble();
    ereg->getdstE()->bubble(RNONE);
    ereg->getdstM()->bubble(RNONE);
    ereg->getsrcA()->bubble(RNONE);
    ereg->getsrcB()->bubble(RNONE); 
}

/*
 * normalE
 * somethin cool
 *
 * @param ereg is nice
 */
void DecodeStage::normalE(E * ereg)
{
    ereg->getstat()->normal();
    ereg->geticode()->normal();
    ereg->getifun()->normal();
    ereg->getvalC()->normal();
    ereg->getvalA()->normal();
    ereg->getvalB()->normal();
    ereg->getdstE()->normal();
    ereg->getdstM()->normal();
    ereg->getsrcA()->normal();
    ereg->getsrcB()->normal();
}

/*
 * calculateControlSignals
 * does stuff
 *
 * @param E_icode is that
 * @param E_dstM is this
 * @param d_srcA is what it is
 * @param d_srcB is src b
 *
 * @return boolean if true
 *
 */
bool DecodeStage::calculateControlSignals(uint64_t E_icode, uint64_t E_dstM, 
                                          uint64_t d_srcA, uint64_t d_srcB, uint64_t E_Cnd)
{
    return ((E_icode == IJXX && !E_Cnd) || 
            ((E_icode == IMRMOVQ || E_icode == IPOPQ) && 
            (E_dstM == d_srcA || E_dstM == d_srcB)));
            
}


/*
 * setEInput
 * Sets the inputs for the next stage, Execute Stage
 *
 * @param ereg is pointer to Execute stage values
 * @param stat value for stat
 * @param icode value for icode
 * @param ifun value for ifun
 * @param dstE value for dstE
 * @param dstM value for dstM
 * @param valC value for valC
 * @param valA value for valA
 * @param valB value for valB
 * @param srcA value for srcA
 * @param srcB value for srcB
 */
void DecodeStage::setEInput(E * ereg, uint64_t stat, uint64_t icode,
                        uint64_t ifun, uint64_t dstE, uint64_t dstM,
                        uint64_t valC, uint64_t valA, uint64_t valB,
                        uint64_t srcA, uint64_t srcB)
{
    ereg->getstat()->setInput(stat);
    ereg->geticode()->setInput(icode);
    ereg->getifun()->setInput(ifun);
    ereg->getdstE()->setInput(dstE);
    ereg->getdstM()->setInput(dstM);
    ereg->getvalC()->setInput(valC);
    ereg->getvalA()->setInput(valA);
    ereg->getvalB()->setInput(valB);
    ereg->getsrcA()->setInput(srcA);
    ereg->getsrcB()->setInput(srcB);
}

/*
 * setSrcA
 * sets srcA for execute stage
 *
 * @param dreg is ptr to decode stage values
 * @param srcA is reference to srcA
 * @param d_icode is value of decode stages' icode
 */
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

/*
 * setSrcB
 * sets srcB for next stage
 *
 * @param dreg is ptr to decode stage
 * @param srcB is reference to srcB
 * @param d_icode is value of icode in decode stage
 */
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

/*
 * setDstE
 * sets dstE for next stage
 *
 * @param dreg is ptr to decode stage
 * @param dstE is reference to dstE
 * @param d_icode is val of ifun in decode
 */
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

/*
 * setDstM
 * sets dstM for next stage
 *
 * @param dreg is ptr to decode stage
 * @param dstM is ref to dstM
 * @param d_icode is value of icode in decode stage
 */
void DecodeStage::setDstM(D * dreg, uint64_t & dstM, uint64_t d_icode)
{
    if (d_icode == IMRMOVQ || d_icode == IPOPQ)
        dstM = dreg->getrA()->getOutput();
    else
        dstM = RNONE;
}

/*
 * setValA
 * SET+FWD A
 *
 * @param valA is ref to valA
 * @param d_srcA is val of srcA in decode
 * @param mreg is ptr to memory stage
 * @param wreg is ptr to writeback stage
 * @param eObj is ptr to Execute stage
 */
void DecodeStage::setValA(uint64_t & valA, uint64_t d_srcA, M * mreg, W * wreg,
                            ExecuteStage * eObj, MemoryStage * mObj, uint64_t icode, uint64_t valP)
{
    RegisterFile * reg = RegisterFile::getInstance();
    bool err = false;
    
    if (icode == ICALL || icode == IJXX)
        valA = valP;
    else if (d_srcA == RNONE)
        valA = 0;
    else if (d_srcA == eObj->getDstE())
        valA = eObj->getValE();
    else if (d_srcA == mreg->getdstM()->getOutput())
        valA = mObj->getValM(); 
    else if (d_srcA == mreg->getdstE()->getOutput())
        valA = mreg->getvalE()->getOutput();
    else if (d_srcA == wreg->getdstM()->getOutput())
        valA = wreg->getvalM()->getOutput();
    else if (d_srcA == wreg->getdstE()->getOutput())
        valA = wreg->getvalE()->getOutput();
    else
        valA = reg->readRegister(d_srcA, err);

}
/*
 * setValB
 * FWD B
 *
 * @param valB is ref to valB
 * @param d_srcB is srcB in decode stage
 * @param mreg is ptr to mem stage
 * @param wreg is ptr to writeback stage
 * @param eObj is ptr to execute stage
 */
void DecodeStage::setValB(uint64_t & valB, uint64_t d_srcB, M * mreg, W * wreg, 
                            ExecuteStage * eObj, MemoryStage * mObj)
{
    RegisterFile * reg = RegisterFile::getInstance();
    bool err = false;        
    
    if (d_srcB == RNONE)
        valB = 0;
    else if (d_srcB == eObj->getDstE())
        valB = eObj->getValE();
    else if (d_srcB == mreg->getdstM()->getOutput())
        valB = mObj->getValM();
    else if (d_srcB == mreg->getdstE()->getOutput())
        valB = mreg->getvalE()->getOutput();
    else if (d_srcB == wreg->getdstM()->getOutput())
        valB = wreg->getvalM()->getOutput();    
    else if (d_srcB == wreg->getdstE()->getOutput())
        valB = wreg->getvalE()->getOutput();
    else
        valB = reg->readRegister(d_srcB, err);    
}

/*
 * getsrcA
 * gets srca
 *
 * @return srcA
 */
uint64_t DecodeStage::getsrcA()
{
    return srcA;
}

/*
 * getsrcB
 * returns srcb
 *
 * @return srcb
 */
uint64_t DecodeStage::getsrcB()
{
    return srcB;
}
