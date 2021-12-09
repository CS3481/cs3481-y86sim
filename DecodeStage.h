class DecodeStage : public Stage
{
    private:
        uint64_t srcA; 
        uint64_t srcB;
        bool bubble;
        void setEInput(E * ereg, uint64_t stat, uint64_t icode,
                        uint64_t ifun, uint64_t dstE, uint64_t dstM,
                        uint64_t valC, uint64_t valA, uint64_t valB,
                        uint64_t srcA, uint64_t srcB);
        void setSrcB(D * dreg, uint64_t & srcB, uint64_t d_icode);
        void setSrcA(D * dreg, uint64_t & srcA, uint64_t d_icode);
        void setDstE(D * dreg, uint64_t & dstE, uint64_t d_icode);
        void setDstM(D * dreg, uint64_t & dstM, uint64_t d_icode);
        void setValA(uint64_t & valA, uint64_t d_srcA, M * mreg, W * wreg, ExecuteStage * eObj, MemoryStage * mObj, uint64_t icode, uint64_t valP);
        void setValB(uint64_t & valB, uint64_t d_srcB, M * mreg, W * wreg, ExecuteStage * eObj, MemoryStage * mObj);
        bool calculateControlSignals(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB, uint64_t E_Cnd);
        void normalE(E * ereg);
        void bubbleE(E * ereg);
    
    public:    
        uint64_t getsrcA();
        uint64_t getsrcB();
        bool doClockLow(PipeReg ** pregs, Stage ** stages);
        void doClockHigh(PipeReg ** pregs);
};
