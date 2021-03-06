class ExecuteStage : public Stage
{
    private:
        uint64_t dstE, valE, Cnd;
        bool bubble;
        void setMInput(M * mreg, uint64_t stat, uint64_t icode, uint64_t Cnd,
                                        uint64_t valE, uint64_t valA, uint64_t dstE, uint64_t dstM);
        void setAluA(E * ereg, uint64_t & aluA, uint64_t e_icode);
        void setAluB(E * ereg, uint64_t & aluB, uint64_t e_icode);
        void setAluFun(uint64_t & alufun, uint64_t e_icode, uint64_t ifun);
        bool setCC(uint64_t e_icode, W * wreg, MemoryStage * mObj);
        void setDstE(E * ereg, uint64_t & dstE, uint64_t e_icode, uint64_t e_Cnd);
        void setConditionCodes(uint64_t valE, uint64_t aluA, uint64_t aluB, uint64_t alufun);
        uint64_t ALU(uint64_t alufun, uint64_t aluA, uint64_t aluB);
        uint64_t cond(uint64_t icode, uint64_t ifun);
        bool calculateControlSignals(MemoryStage * mObj, W * wreg);
        void normalM(M * mreg);
        void bubbleM(M * mreg);

    public:
        bool doClockLow(PipeReg ** pregs, Stage ** stages);
        void doClockHigh(PipeReg ** pregs);
        uint64_t getDstE();
        uint64_t getValE();
        uint64_t getCnd();
};
