#include <string>
#include <cstdint>
#include "RegisterFile.h"
#include "PipeRegField.h"
#include "PipeReg.h"
#include "E.h"
#include "M.h"
#include "W.h"
#include "Stage.h"
#include "MemoryStage.h"
#include "ExecuteStage.h"
#include "Status.h"
#include "Debug.h"
#include "Instructions.h"
#include "ConditionCodes.h"
#include "Tools.h"


/*
 * doClockLow
 * Low clock cycle for execute stage
 *
 * @param pregs is ptr to pipe reg class
 * @param stages is ptr to other stages
 *
 * @return boolean false if no errors
 */
bool ExecuteStage::doClockLow(PipeReg ** pregs, Stage ** stages)
{
    E * ereg = (E *) pregs[EREG];
    M * mreg = (M *) pregs[MREG];
    W * wreg = (W *) pregs[WREG];
    MemoryStage * mObj = (MemoryStage *) stages[MSTAGE];
    
    uint64_t icode = ereg->geticode()->getOutput(), valA = ereg->getvalA()->getOutput(), 
            dstM = ereg->getdstM()->getOutput(), 
            stat = ereg->getstat()->getOutput(), ifun = ereg->getifun()->getOutput();
    
    Cnd = 0;
    dstE = ereg->getdstE()->getOutput();
    valE = ereg->getvalC()->getOutput();

    uint64_t aluA;
    uint64_t aluB;
    uint64_t alufun;
    bool set_cc;
    
    setAluA(ereg, aluA, icode);
    setAluB(ereg, aluB, icode);
    setAluFun(alufun, icode, ifun);
    set_cc = setCC(icode, wreg, mObj);

    valE = ALU(alufun, aluA, aluB);

    if (set_cc)
    {
        setConditionCodes(valE, aluA, aluB, alufun);   
    } 
    
    Cnd = cond(icode, ifun);
    setDstE(ereg, dstE, icode, Cnd);
        
    setMInput(mreg, stat, icode, Cnd, valE, valA, dstE, dstM);
    
    bubble = calculateControlSignals(mObj, wreg);
     
    return false;

}

/*
 * doClockHigh
 * high clock cycle for execute stage
 *
 * @param pregs is ptr to pipe reg class
 */
void ExecuteStage::doClockHigh(PipeReg ** pregs)
{
    M * mreg = (M *) pregs[MREG];
    
    if (bubble)
        bubbleM(mreg);
    else
        normalM(mreg);
}

/*
 * normal
 * regular action from doclockhigh
 *
 * @param mreg is somethin
 */
void ExecuteStage::normalM(M * mreg)
{
    mreg->getstat()->normal();
    mreg->geticode()->normal();
    mreg->getCnd()->normal();
    mreg->getvalE()->normal();
    mreg->getvalA()->normal();
    mreg->getdstE()->normal();
    mreg->getdstM()->normal();
}

/*
 * bubble
 * bubble action for doclockhigh
 *
 * @param mreg is that
 */
void ExecuteStage::bubbleM(M * mreg)
{
    mreg->getstat()->bubble(SAOK);
    mreg->geticode()->bubble(INOP);
    mreg->getCnd()->bubble();
    mreg->getvalE()->bubble();
    mreg->getvalA()->bubble();
    mreg->getdstE()->bubble(RNONE);
    mreg->getdstM()->bubble(RNONE);
}

/*
 * calculateControlSignals
 * does that
 *
 * @return boolean if true
 */
bool ExecuteStage::calculateControlSignals(MemoryStage * mObj, W * wreg)
{
    uint64_t m_stat = mObj->getStat();
    uint64_t W_stat = wreg->getstat()->getOutput();

    if ((m_stat == SADR || m_stat == SINS || m_stat == SHLT) || 
        (W_stat == SADR || W_stat == SINS || W_stat == SHLT))
    {
        return true;
    }
    else
        return false;
}

/*
 * setMInput
 * sets input for next stage
 *
 * @param mreg is val of mreg
 * @param stat is val of stat
 * @param icode new val of icode
 * @param cnd new val of cnd
 * @param valE new val of valE
 * @param valA new val of valA
 * @param dstE new val of dstE
 * @param dstM new val of dstM
 */
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

/*
 * cond
 * block in exec stage to perform rrmoq/cmovxx
 *
 * @param icode
 * @param ifun
 * 
 * @return the value of e_Cnd
 */
uint64_t ExecuteStage::cond(uint64_t icode, uint64_t ifun) 
{
    ConditionCodes * cc = ConditionCodes::getInstance();
    bool err = false;
    bool sf = cc->getConditionCode(SF, err);
    bool of = cc->getConditionCode(OF, err);
    bool zf = cc->getConditionCode(ZF, err);

    //base case
    if ((icode != IJXX) && (icode != ICMOVXX))
    {
        return 0;
    }
    
    //unconiditonal jump/move
    if (ifun == UNCOND)
    {
        return 1;
    }
    
    //less than or equal to zero
    if (ifun == LESSEQ)
    {
        return (sf ^ of) | zf;
    }
    
    //less than zero
    if (ifun == LESS)
    {
        return (sf ^ of);
    }

    //equal to zero
    if (ifun == EQUAL)
    {
        return zf;
    }

    //not equal to zero
    if (ifun == NOTEQUAL)
    {
        return !zf;
    }

    //greater than zero
    if (ifun == GREATER)
    {
        return (!sf ^ of) & !zf;
    }

    //greater than or equal to zero
    if (ifun == GREATEREQ)
    {
        return !(sf^of);
    }
    
    return 0;
}

/*
 * setAluA
 * sets val of aluA
 *
 * @param ereg is ptr to E class
 * @param aluA is reference to aluA
 * @param e_icode is val of icode in execute stage
 */
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

/*
 * setAluB
 * sets val of aluB
 *
 * @param ereg is ptr to E class
 * @param aluB reference to aluB
 * @param e_icode val of icode in execute stage
 *
 */
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

/*
 * setAluFun
 * sets val of alu function
 *
 * @param alufun is ref to alufun
 * @param e_icode is icode val in exec stage
 * @param ifun is val of ifun
 */
void ExecuteStage::setAluFun(uint64_t & alufun, uint64_t e_icode, uint64_t ifun)
{
    if (e_icode == IOPQ)
        alufun = ifun;
    else 
        alufun = ADDQ;
}

/*
 * setCC
 * set val of setcc
 *
 * @param set_cc is ref to set_cc
 * @param e_icode is val of icode in execute stag
 */
bool ExecuteStage::setCC(uint64_t e_icode, W * wreg, MemoryStage * mObj)
{
    uint64_t m_stat = mObj->getStat();
    uint64_t W_stat = wreg->getstat()->getOutput();

    if ((e_icode == IOPQ) && 
        (m_stat != SADR && m_stat != SINS && m_stat != SHLT) && 
        (W_stat != SADR && W_stat != SINS && W_stat != SHLT))
    {
        return true;
    }
    else
        return false;
}

/*
 * setDstE
 * sets val of dstE
 *
 * @param dstE is ref to dstE
 * @param e_icode is val of icode in execute stage
 * @param e_cnd is val of cnd in execute stage
 */
void ExecuteStage::setDstE(E * ereg, uint64_t & dstE, uint64_t e_icode, uint64_t e_Cnd)
{
    if (e_icode == IRRMOVQ && !e_Cnd)
        dstE = RNONE;
    else
        dstE = ereg->getdstE()->getOutput();
}

/*
 * setConditionCodes
 * sets conidtion code flags
 *
 * @param valE is valE
 * @param aluA is aluA
 * @param aluB is val of aluB
 * @param alufun is val of alu function
 */
void ExecuteStage::setConditionCodes(uint64_t valE, uint64_t aluA, uint64_t aluB, uint64_t alufun)
{
    ConditionCodes * cc = ConditionCodes::getInstance();
    bool err = false;

    //ZF (Zero flag)
    if (valE == 0)
        cc->setConditionCode(1, ZF, err);
    else
        cc->setConditionCode(0, ZF, err);
    
    //SF (Sign Flag)
    if (Tools::sign(valE))
        cc->setConditionCode(1, SF, err);
    else
        cc->setConditionCode(0, SF, err);
    
    //OF (Overflow flag)
    if (alufun == ADDQ)
    {
        if (Tools::addOverflow(aluB, aluA))
            cc->setConditionCode(1, OF, err);
        else
            cc->setConditionCode(0, OF, err);
    }
    else if (alufun == SUBQ)
    {
         if (Tools::subOverflow(aluB, aluA))
            cc->setConditionCode(1, OF, err);
         else
            cc->setConditionCode(0, OF, err);
    }
}

/*
 * ALU
 * box for ALU in y86
 *
 * @param alufun is function for alu
 * @param A is val of A
 * @param B is val of B
 *
 * @return result of ALU box
 */
uint64_t ExecuteStage::ALU(uint64_t alufun, uint64_t A, uint64_t B)
{
    if (alufun == ADDQ)
    {
        return (B + A);
    }

    if (alufun == SUBQ)
    {
        return (B - A);
    }

    if (alufun == XORQ)
    {
        return (B ^ A);
    }

    if (alufun == ANDQ)
    {
        return (B & A);
    }
    else
    {
        return 0;
    }
    
}

/*
 * getDstE
 * returns dstE
 *
 * @return dstE
 */
uint64_t ExecuteStage::getDstE()
{
    return dstE;
}

/*
 * getValE
 * returns valE
 * 
 * @return valE
 */
uint64_t ExecuteStage::getValE()
{
    return valE;
}

uint64_t ExecuteStage::getCnd()
{
    return Cnd;
}
