class DecodeStage : public Stage
{
    private:
        void setEInput(E * ereg, uint64_t stat, uint64_t icode,
                        uint64_t ifun, uint64_t dstE, uint64_t dstM,
                        uint64_t valC, uint64_t valA, uint64_t valB,
                        uint64_t srcA, uint64_t srcB);
        void setSrcB(D * dreg, uint64_t & srcB, uint64_t d_icode);
        void setSrcA(D * dreg, uint64_t & srcA, uint64_t d_icode);
        void setDstE(D * dreg, uint64_t & dstE, uint64_t d_icode);
        void setDstM(D * dreg, uint64_t & dstM, uint64_t d_icode);
        void setValA(uint64_t & valA, uint64_t d_srcA, M * mreg, W * wreg, ExecuteStage * eObj);
        void setValB(uint64_t & valB, uint64_t d_srcB, M * mreg, W * wreg, ExecuteStage * eObj);

    public:
        bool doClockLow(PipeReg ** pregs, Stage ** stages);
        void doClockHigh(PipeReg ** pregs);
};
