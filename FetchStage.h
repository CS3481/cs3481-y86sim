//class to perform the combinational logic of
//the Fetch stage
class FetchStage: public Stage
{
   private:
      bool F_stall;
      bool D_stall;
      bool D_bubble;
      void setDInput(D * dreg, uint64_t stat, uint64_t icode, uint64_t ifun, 
                     uint64_t rA, uint64_t rB,
                     uint64_t valC, uint64_t valP);
      uint64_t selectPC(F * freg, M * mreg, W * wreg, uint64_t f_pc);
      bool needValC(uint64_t f_icode);
      bool needRegIds(uint64_t f_icode);
      uint64_t predictPC(uint64_t f_icode, uint64_t f_valC, uint64_t f_valP);
      uint64_t PCincrement(uint64_t f_pc, bool checkNeedIds, bool checkNeedValC);
      void getRegIds(uint64_t & rA, uint64_t & rB, uint64_t f_pc);
      uint64_t buildValC(uint64_t f_pc, bool needRegIds);
      bool instrValid(uint64_t icode);
      uint64_t setStat(uint64_t icode, bool mem_error);
      void calcFStall(uint64_t D_icode, uint64_t M_icode, uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB); 
      void calcDStall(uint64_t E_icode, uint64_t E_dstM, uint64_t d_srcA, uint64_t d_srcB);
      void calculateControlSignals(PipeReg ** pregs, Stage ** stages);
      bool calcBubble(uint64_t D_icode, uint64_t M_icode, uint64_t E_icode, uint64_t e_Cnd, uint64_t d_srcA, uint64_t d_srcB, uint64_t E_dstM);
      void bubbleD(PipeReg ** pregs);
      void normalD(PipeReg ** pregs);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);

};
