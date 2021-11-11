class ExecuteStage : public Stage
{
    private:
        void setMInput(M * mreg, uint64_t stat, uint64_t icode, uint64_t Cnd,
                                        uint64_t valE, uint64_t valA, uint64_t dstE, uint64_t dstM);
        void setAluA(E * ereg, uint64_t & aluA, uint64_t e_icode);
        void setAluB(E * ereg, uint64_t & aluB, uint64_t e_icode);
        void setAluFun(uint64_t & alufun, uint64_t e_icode, uint64_t ifun);
        void setCC(bool & set_cc, uint64_t e_icode);
        void setDstE(E * ereg, uint64_t & dstE, uint64_t e_icode, uint64_t e_Cnd);
    public:
        bool doClockLow(PipeReg ** pregs, Stage ** stages);
        void doClockHigh(PipeReg ** pregs);
};
