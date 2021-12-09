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
      void calcFStall(DecodeStage * dObj, E * ereg, D * dreg, M * mreg); 
      void calcDStall(DecodeStage * dObj, E * ereg);
      void calculateControlSignals(DecodeStage * dObj, ExecuteStage * eObj, E * ereg, D * dreg, M * mreg);
      void calcBubble(DecodeStage * dObj, ExecuteStage * eObj, E * ereg, D * dreg, M * mreg);
      void bubbleD(PipeReg ** pregs);
      void normalD(PipeReg ** pregs);
   public:
      bool doClockLow(PipeReg ** pregs, Stage ** stages);
      void doClockHigh(PipeReg ** pregs);

};
