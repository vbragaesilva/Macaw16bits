#pragma once
#include "RAM.h"
#include "Register.h"
#include "Bus.h"
#include "ALU.h"
#include "Files.h"
#include <map>
#undef OUT

struct CPU
{
public://ENUMS
  enum AssemblerFlags {
        LINES = 1,
        BINARY = 2,
        SAVE = 4,
        LOAD = 8
    };
  enum LogLevel
    {
        NoLog, LogInstructions, LogInstructionsBinary, LogMicroInstructions
    };
  enum Registers
    {
        RA, RB, RC, RD, RE,
        RO = 255
    };
  std::map<String, Registers> RegMap;// more on init
  enum Instruction {
    NOP,
    LARB, LARW, LDI, LAB, LAW, LDR, 
    SARL, SARU, SARW, STL, STU, STW, STI,
    JMP, JC, JNC, JZ, JNZ, JN, JNN,
    ADD, ADDA, ADDI, ADDR, SUB, SUBA, SUBI, SUBR, MULT, DIV,
    INC, DEC,
    PUSH, POP,
    PRNT = 253, OUT, HLT
  };
  std::map<String, Instruction> Opcode;// more on init
  enum FileType
    {
        X = 1
    };
private:
  enum instructionType
    {
        U,    // Undetermined
        P,    // Pure,               1 byte
        IB,   // Ins+Byte,           2 bytes
        IR,   // Ins+Reg,            2 bytes
        IBB,  // Ins+Byte+Byte,      3 bytes
        IRR,  // Ins+Reg+Reg,        3 bytes
        IW,
        IL,// Ins+Word,           3 bytes
        IBBB, // Ins+Byte+Byte+Byte, 4 bytes
        IRRR, // Ins+Reg+Reg+Reg,    4 bytes
        IBW,  // Ins+Byte+Word,      4 bytes
        IRW,  // Ins+Reg+Word,       4 bytes
        IWB,  // Ins+Word+Byte,      4 bytes
        IWR,  
    };
  LogLevel m_level = NoLog;
  int64_t m_delayUSeconds;
  bool halt;
  Reader reader;
  AssemblerLineReader lineReader;
  Writer writer;
  Byte InstructionBytes[256];
  instructionType InsTypes[256];
  String loadedProgram = "";
  Word* exitCode = &E.Value;
  bool printNOP = false;
public:
  Bus bus;
  ALU alu;
  RAM ram;
  Register A;        // Register 0
  Register B;        // Register 1
  Register C;        // Register 2
  Register D;        // Register 3
  Register E;        // Register 4, will be used to exitCode
  Register H;        // Comparison Register
  Register SP;       // Stack Pointer
  OutputRegister O;  // Output Register
  ProgramCounter PC; // Program Counter
  InstructionRegister I;
  FlagRegister FR;
  StepCounter SC;

  //Init Functions
  CPU()
        :m_delayUSeconds(0), m_level(NoLog)
    {
        Init();
    }
  CPU(const int64_t& delayUSeconds, const LogLevel& level = NoLog)
        :m_delayUSeconds(delayUSeconds), m_level(level)
    {
        Init();
    }
  void Init()
  {
    //REGISTERS AND COMPONENTS
    //Init components
    {
      halt = true;
      ram.ConnectBus(bus);
      alu.ConnectRegisters(A, B);
      alu.ConnectBusAndFlag(bus, FR);
      A.Connect(bus);
      B.Connect(bus);
      C.Connect(bus);
      D.Connect(bus);
      E.Connect(bus);
      H.Connect(bus);
      SP.Connect(bus);
      O.Connect(bus);
      PC.Connect(bus);
      I.Connect(bus);
    }
    //Init Register Map
    {
      RegMap["RA"] = RA;
      RegMap["RB"] = RB;
      RegMap["RC"] = RC;
      RegMap["RD"] = RD;
      RegMap["RE"] = RE;
      RegMap["RO"] = RO;
    }

    //INSTRUCTIONS
    //Init InstructionBytes
    for (int i = 0; i < 256; i++)
    {
      InstructionBytes[i] = 0;
      InsTypes[i] = U;
    }
    //Init InstructionBytes values
    {
      InstructionBytes[NOP] = 0;
      InstructionBytes[LARB] = 2;
      InstructionBytes[LARW] = 2;
      InstructionBytes[LDI] = 3;
      InstructionBytes[LAB] = 3;
      InstructionBytes[LAW] = 3;
      InstructionBytes[LDR] = 2;
      InstructionBytes[SARL] = 2;
      InstructionBytes[SARU] = 2;
      InstructionBytes[SARW] = 2;
      InstructionBytes[STL] = 3;
      InstructionBytes[STU] = 3;
      InstructionBytes[STW] = 3;
      InstructionBytes[STI] = 3;

      InstructionBytes[JMP] = 2;
      InstructionBytes[JC] = 2;
      InstructionBytes[JNC] = 2;
      InstructionBytes[JZ] = 2;
      InstructionBytes[JNZ] = 2;
      InstructionBytes[JN] = 2;
      InstructionBytes[JNN] = 2;

      InstructionBytes[ADD] = 1;
      InstructionBytes[ADDA] = 3;
      InstructionBytes[ADDI] = 3;
      InstructionBytes[ADDR] = 3;
      InstructionBytes[SUB] = 1;
      InstructionBytes[SUBA] = 3;
      InstructionBytes[SUBI] = 3;
      InstructionBytes[SUBR] = 3;
      InstructionBytes[MULT] = 1;
      InstructionBytes[DIV] = 1;
      InstructionBytes[INC] = 1;
      InstructionBytes[DEC] = 1;

      InstructionBytes[PUSH] = 1;
      InstructionBytes[POP] = 1;

      InstructionBytes[PRNT] = 0;
      InstructionBytes[OUT] = 0;
      InstructionBytes[HLT] = 0;
    }
    //Init InsTypes
    {
      InsTypes[NOP] = P;
      InsTypes[LARB] = IRR;
      InsTypes[LARW] = IRR;
      InsTypes[LDI] = IRW;
      InsTypes[LAB] = IRW;
      InsTypes[LAW] = IRW;
      InsTypes[LDR] = IRR;
      InsTypes[SARL] = IRR;
      InsTypes[SARU] = IRR;
      InsTypes[SARW] = IRR;
      InsTypes[STL] = IRW;
      InsTypes[STU] = IRW;
      InsTypes[STW] = IRW;
      InsTypes[STI] = IBW;
      InsTypes[JMP] = IL;
      InsTypes[JC] = IL;
      InsTypes[JNC] = IL;
      InsTypes[JZ] = IL;
      InsTypes[JNZ] = IL;
      InsTypes[JN] = IL;
      InsTypes[JNN] = IL;
      InsTypes[ADD] = IR;
      InsTypes[ADDA] = IWR;
      InsTypes[ADDI] = IWR;
      InsTypes[ADDR] = IRRR;
      InsTypes[SUB] = IR;
      InsTypes[SUBA] = IWR;
      InsTypes[SUBI] = IWR;
      InsTypes[SUBR] = IRRR;
      InsTypes[MULT] = IR;
      InsTypes[DIV] = IR;
      InsTypes[INC] = IR;
      InsTypes[DEC] = IR;
      InsTypes[PUSH] = IR;
      InsTypes[POP] = IR;

      InsTypes[PRNT] = P;
      InsTypes[OUT] = P;
      InsTypes[HLT] = P;

    }
    //Init Opcode Instruction Map
    {
      Opcode["NOP"] = NOP;
      Opcode["LARB"] = LARB;
      Opcode["LARW"] = LARW;
      Opcode["LDI"] = LDI;
      Opcode["LAB"] = LAB;
      Opcode["LAW"] = LAW;
      Opcode["LDR"] = LDR;
      Opcode["SARL"] = SARL;
      Opcode["SARU"] = SARU;
      Opcode["SARW"] = SARW;
      Opcode["STL"] = STL;
      Opcode["STU"] = STU;
      Opcode["STW"] = STW;
      Opcode["STI"] = STI;
      Opcode["JMP"] = JMP;
      Opcode["JC"] = JC;
      Opcode["JNC"] = JNC;
      Opcode["JZ"] = JZ;
      Opcode["JNZ"] = JNZ;
      Opcode["JN"] = JN;
      Opcode["JNN"] = JNN;
      Opcode["ADD"] = ADD;
      Opcode["ADDA"] = ADDA;
      Opcode["ADDI"] = ADDI;
      Opcode["ADDR"] = ADDR;
      Opcode["SUB"] = SUB;
      Opcode["SUBA"] = SUBA;
      Opcode["SUBI"] = SUBI;
      Opcode["SUBR"] = SUBR;
      Opcode["MULT"] = MULT;
      Opcode["DIV"] = DIV;
      Opcode["INC"] = INC;
      Opcode["DEC"] = DEC;
      Opcode["PUSH"] = PUSH;
      Opcode["POP"] = POP;

      Opcode["PRNT"] = PRNT;
      Opcode["OUT"] = OUT;
      Opcode["HLT"] = HLT;
    }
    

  }

    //Computing Functions
  void Exec()
  {
    Byte Step = SC.Value;
    Byte Ins = I[0];
    Byte nBytes = InstructionBytes[Ins];

    if (Step == 0)
    {
      std_FetchAddress();
      return;
    }
    if (Step == 1)
    {
      std_FetchInstruction();
      return;
    }

    if (Step < nBytes + 2)
    {
      if (Step == 2)
      {
        std_FetchAddressAndByte(1);
        return;
      }
      if (nBytes == 2 or nBytes == 3)
      {
        if (Step == 3)
        {
          std_FetchAddressAndByte(2);
          return;
        }
      }
      if (nBytes == 3)
      {
        if (Step == 4)
        {
          std_FetchAddressAndByte(3);
          return;
        }
      }

    }
    //next Step is nBytes + 2
    switch (Ins) {
    case NOP:
      SC.EndOperation();
      if (printNOP) std::cout << "NOP" << '\n';
      return;
    case LARB:
      switch (Step)
      {
      case 4:
        ram.SetBank(1);
        std_registerOut(I[2]);
        ram.MIn();
        bus.Reset();
        return;
      case 5:
        ram.OutByte();
        std_registerIn(I[1]);
        bus.Reset();
        ram.SetBank(0);
        return;
      case 6:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case LARW:
      switch (Step)
      {
      case 4:
        ram.SetBank(1);
        std_registerOut(I[2]);
        ram.MIn();
        bus.Reset();
        return;
      case 5:
        ram.OutWordLittle();
        std_registerIn(I[1]);
        bus.Reset();
        ram.SetBank(0);
        return;
      case 6:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case LDI:
      switch (Step)
      {
      case 5:
        I.OutWordLittle(2);
        std_registerIn(I[1]);
        bus.Reset();
        return;
      case 6:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case LAB:
      switch (Step)
      {
      case 5:
        ram.SetBank(1);
        I.OutWordLittle(2);
        ram.MIn();
        bus.Reset();
        return;
      case 6:
        ram.OutByte();
        std_registerIn(I[1]);
        bus.Reset();
        return;
      case 7:
        ram.SetBank(0);
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case LAW:
      switch (Step)
      {
      case 5:
        ram.SetBank(1);
        I.OutWordLittle(2);
        ram.MIn();
        bus.Reset();
        return;
      case 6:
        ram.OutWordLittle();
        std_registerIn(I[1]);
        bus.Reset();
        return;
      case 7:
        ram.SetBank(0);
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case LDR:
      switch (Step)
      {
      case 4:
        std_registerOut(I[2]);
        std_registerIn(I[1]);
        bus.Reset();
        return;
      case 5:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case SARL:
      switch (Step)
      {
      case 4:
        ram.SetBank(1);
        std_registerOut(I[2]);
        ram.MIn();
        bus.Reset();
        return;
      case 5:
        std_registerOut(I[1]);
        ram.InLower();
        bus.Reset();
        ram.SetBank(0);
        return;
      case 6:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case SARU:
      switch (Step)
      {
      case 4:
        ram.SetBank(1);
        std_registerOut(I[2]);
        ram.MIn();
        bus.Reset();
        return;
      case 5:
        std_registerOut(I[1]);
        ram.InUpper();
        bus.Reset();
        ram.SetBank(0);
        return;
      case 6:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case SARW:
      switch (Step)
      {
      case 4:
        ram.SetBank(1);
        std_registerOut(I[2]);
        ram.MIn();
        bus.Reset();
        return;
      case 5:
        std_registerOut(I[1]);
        ram.InWord();
        bus.Reset();
        ram.SetBank(0);
        return;
      case 6:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case STL:
      switch (Step)
      {
      case 5:
        ram.SetBank(1);
        I.OutWordLittle(2);
        ram.MIn();
        bus.Reset();
        return;
      case 6:
        std_registerOut(I[1]);
        ram.InLower();
        return;
      case 7:
        ram.SetBank(0);
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case STU:
      switch (Step)
      {
      case 5:
        ram.SetBank(1);
        I.OutWordLittle(2);
        ram.MIn();
        bus.Reset();
        return;
      case 6:
        std_registerOut(I[1]);
        ram.InUpper();
        return;
      case 7:
        ram.SetBank(0);
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case STW:
      switch (Step)
      {
      case 5:
        ram.SetBank(1);
        I.OutWordLittle(2);
        ram.MIn();
        bus.Reset();
        return;
      case 6:
        std_registerOut(I[1]);
        ram.InWord();
        bus.Reset();
        return;
      case 7:
        ram.SetBank(0);
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case STI:
      switch (Step)
      {
      case 5:
        ram.SetBank(1);
        I.OutWordLittle(2);
        ram.MIn();
        bus.Reset();
        return;
      case 6:
        I.OutByte(1);
        ram.InLower();
        bus.Reset();
        return;
      case 7:
        ram.SetBank(0);
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case JMP:
      switch (Step)
      {
      case 4:
        I.OutWordLittle(1);
        PC.In();
        bus.Reset();
        return;
      case 5:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case JC:
      switch (Step)
      {
      case 4:
        if (FR.getFlag(FlagRegister::CARRY))
        {
          I.OutWordLittle(1);
          PC.In();
          bus.Reset();
        }
        return;
      case 5:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case JNC:
      switch (Step)
      {
      case 4:
        if (!FR.getFlag(FlagRegister::CARRY))
        {
          I.OutWordLittle(1);
          PC.In();
          bus.Reset();
        }
        return;
      case 5:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case JZ:
      switch (Step)
      {
      case 4:
        if (FR.getFlag(FlagRegister::ZERO))
        {
          I.OutWordLittle(1);
          PC.In();
          bus.Reset();

        }
        return;
      case 5:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case JNZ:
      switch (Step)
      {
      case 4:
        if (!FR.getFlag(FlagRegister::ZERO))
        {
          I.OutWordLittle(1);
          PC.In();
          bus.Reset();
        }
        return;
      case 5:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case JN:
      switch (Step)
      {
      case 4:
        if (FR.getFlag(FlagRegister::NEGATIVE))
        {
          I.OutWordLittle(1);
          PC.In();
          bus.Reset();
        }
        return;
      case 5:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case JNN:
      switch (Step)
      {
      case 4:
        if (!FR.getFlag(FlagRegister::NEGATIVE))
        {
          I.OutWordLittle(1);
          PC.In();
          bus.Reset();
        }
        return;
      case 5:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case ADD:
      switch (Step)
      {
      case 3:
        alu.OutSum();
        std_registerIn(I[1]);
        bus.Reset();
        return;
      case 4:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case ADDA:
      switch (Step)
      {
      case 5:
        I.OutWordLittle(1);
        ram.MIn();
        bus.Reset();
        return;
      case 6:
        ram.OutWordLittle();
        B.In();
        bus.Reset();
        return;
      case 7:
        alu.OutSum();
        std_registerIn(I[3]);
        bus.Reset();
        return;
      case 8:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case ADDI:
      switch (Step)
      {
      case 5:
        I.OutWordLittle(1);
        B.In();
        bus.Reset();
        return;
      case 6:
        alu.OutSum();
        std_registerIn(I[3]);
        bus.Reset();
        return;
      case 7:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case ADDR:
      switch (Step)
      {
      case 5:
        std_registerOut(I[1]);
        A.In();
        bus.Reset();
        return;
      case 6:
        std_registerOut(I[2]);
        B.In();
        bus.Reset();
        return;
      case 7:
        alu.OutSum();
        std_registerIn(I[3]);
        bus.Reset();
        return;
      case 8:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case SUB:
      switch (Step)
      {
      case 3:
        alu.OutSub();
        std_registerIn(I[1]);
        bus.Reset();
        return;
      case 4:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case SUBA:
      switch (Step)
      {
      case 5:
        I.OutWordLittle(1);
        ram.MIn();
        bus.Reset();
        return;
      case 6:
        ram.OutWordLittle();
        B.In();
        bus.Reset();
        return;
      case 7:
        alu.OutSub();
        std_registerIn(I[3]);
        bus.Reset();
        return;
      case 8:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case SUBI:
      switch (Step)
      {
      case 5:
        I.OutWordLittle(1);
        B.In();
        bus.Reset();
        return;
      case 6:
        alu.OutSub();
        std_registerIn(I[3]);
        bus.Reset();
        return;
      case 7:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case SUBR:
      switch (Step)
      {
      case 5:
        std_registerOut(I[1]);
        A.In();
        bus.Reset();
        return;
      case 6:
        std_registerOut(I[2]);
        B.In();
        bus.Reset();
        return;
      case 7:
        alu.OutSub();
        std_registerIn(I[3]);
        bus.Reset();
        return;
      case 8:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case MULT:
      switch (Step)
      {
      case 3:
        alu.OutMult();
        std_registerIn(I[1]);
        bus.Reset();
        return;
      case 4:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case DIV:
      switch (Step)
      {
      case 3:
        bus.Value = 0xFF;
        std_registerIn(I[1]);
        bus.Reset();
        return;
      case 4:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case INC:
      switch (Step)
      {
      case 3:
        if (std_registerGetValue(I[1]) == 0xFFFF)
        {
          FR.setFlag(FlagRegister::CARRY, 1);
        }
        std_registerINC(I[1]);
        return;
      case 4:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case DEC:
      switch (Step)
      {
      case 3:
        if (std_registerGetValue(I[1]) == 1)
        {
          FR.setFlag(FlagRegister::ZERO, 1);
        }
        else if (std_registerGetValue(I[1]) == 0)
        {
          FR.setFlag(FlagRegister::NEGATIVE, 1);
        }
        std_registerDEC(I[1]);
        return;
      case 4:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case PUSH:
      switch (Step)
      {
      case 3:
        ram.SetBank(2);
        SP.Out();
        ram.MIn();
        bus.Reset();
        return;
      case 4:
        std_registerOut(I[1]);
        ram.InWord();
        bus.Reset();
        ram.SetBank(0);
        SP.Value++;
        SP.Value++;//Two increments because we push a word
        return;
      case 5:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case POP:
      if (SP.Value == 0)
      {
        std::cout << "Stack has no data pushed!" << '\n';
        return;
      }
      switch (Step)
      {
      case 3:
        SP.Value--;
        SP.Value--;//Two decrements because we pop a word
        ram.SetBank(2);
        SP.Out();
        ram.MIn();
        bus.Reset();
        return;
      case 4:
        ram.OutWordLittle();
        std_registerIn(I[1]);
        bus.Reset();
        ram.SetBank(0);
        return;
      case 5:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case PRNT:
      switch (Step)
      {
      case 2:
        std::cout << (char)(O.Value & 0b0000000011111111);
        return;
      case 3:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case OUT:
      switch (Step)
      {
      case 2:
        O.Output();
        return;
      case 3:
        SC.EndOperation();
        return;
      default:
        std_NoMicroLog(Step, Ins);
        return;
      }
    case HLT:
      halt = true;
      std::cout << '\n' << loadedProgram << " exited with code " << exitCode[0] << '\n';
      return;
    default:
      std::cout << "No instruction " << (int)Ins << " with step " << (int)Step << '\n';
      std::cout << "I: " << (int)I[0] << " " << (int)I[1] << " " << (int)I[2] << " " << (int)I[3] << '\n';
    }    
  }
  void Run()
  {
    ram.SetBank(0);
    bus.Value = (ram[0xfffd] << 8) + ram[0xfffc];
    PC.In();
    bus.Reset();
    while (!halt) {
      Exec();
      SC++;
      if (m_delayUSeconds != 0) usleep(m_delayUSeconds);
    }
  }

  //Standard Push
  void PushByteRAM(const Byte& b, const Byte& bank_number)
    {
        ram.SetBank(bank_number);
        ram.PushByte(b);
        ram.SetBank(0);
    }
  void PushWordRAM(const Word& w, const Byte& bank_number)
    {
        ram.SetBank(bank_number);
        ram.PushWord(w);
        ram.SetBank(0);
    }

  //Push Instructions
  void PushInstruction(const Byte& Ins)
    {
        ram.SetBank(0);
        ram.PushByte(Ins);
    }
  void PushInstructionByteWord(const Byte& Ins, const Byte& reg=0, const Word& value=0)
    {
        ram.SetBank(0);
        ram.PushByte(Ins);
        ram.PushByte(reg);
        ram.PushWord(value);
    }
  void PushInstructionWordByte(const Byte& Ins, const Word& w, const Byte& reg)
    {
        ram.SetBank(0);
        ram.PushByte(Ins);
        ram.PushWord(w);
        ram.PushByte(reg);
    }
  void PushInstructionWord(const Byte& Ins, const Word& w)
    {
        ram.SetBank(0);
        ram.PushByte(Ins);
        ram.PushWord(w);
    }
  void PushInstructionByte(const Byte& Ins, const Byte& b)
    {
        ram.SetBank(0);
        ram.PushByte(Ins);
        ram.PushByte(b);
    }
  void PushInstructionByte2(const Byte& Ins, const Byte& b1, const Byte& b2)
    {
        ram.SetBank(0);
        ram.PushByte(Ins);
        ram.PushByte(b1);
        ram.PushByte(b2);
    }
  void PushInstructionByte3(const Byte& Ins, const Byte& b1, const Byte& b2, const Byte& b3)
    {
        ram.SetBank(0);
        ram.PushByte(Ins);
        ram.PushByte(b1);
        ram.PushByte(b2);
        ram.PushByte(b3);
    }

  //Push Data
  void PushDataByte(const Byte& b)
    {
        ram.SetBank(1);
        ram.PushByte(b);
        ram.SetBank(0);
    }
  void PushDataString(const String& s)
  {
    ram.SetBank(1);
    for (int i = 0; i < s.length(); i++)
    {
      ram.PushByte((Byte)s[i]);
    }
    ram.SetBank(0);
  }
  void PushDataWord(const Word& w)
    {
        ram.SetBank(1);
        ram.PushWord(w);
        ram.SetBank(0);
    }

  //Load Functions
  void LoadXFile(const char* path, Word offset = 0)
  {
    loadedProgram = path;
    String filepath = String("C:/Dev/Macaw16/HD/") + String(path);
    reader.LoadFile(filepath.c_str());
    std::vector<Byte> bytes;
    bytes.reserve(reader.length);
    bytes = reader.ReadFile();
    LoadXByteArray(bytes);

  }

  //Standart Functions
  void std_FetchAddress()
  {
    PC.Out();
    ram.MIn();
    bus.Reset();
    PC++;
  }
  void std_FetchInstruction()
  {
    ram.OutByte();
    I.InByte(0);
    bus.Reset();
  }
  void std_FetchByte(const Byte& index)
  {
    ram.OutByte();
    I.InByte(index);
    bus.Reset();
  }
  void std_FetchAddressAndByte(const Byte& index)
  {
    PC.Out();
    ram.MIn();
    bus.Reset();
    PC++;

    ram.OutByte();
    I.InByte(index);
    bus.Reset();
  }
  void std_NoMicroLog(const Byte& step, const Byte& ins)
  {
    std::cout << "No step " << (int)step << " for instruction " << (int)ins << '\n';
  }
  void std_registerIn(const Byte& reg)
  {
    if (reg == RA) A.In();
    if (reg == RB) B.In();
    if (reg == RC) C.In();
    if (reg == RD) D.In();
    if (reg == RE) E.In();
    if (reg == RO) O.In();

  }
  void std_registerOut(const Byte& reg)
  {
    if (reg == RA) A.Out();
    if (reg == RB) B.Out();
    if (reg == RC) C.Out();
    if (reg == RD) D.Out();
    if (reg == RE) E.Out();
    if (reg == RO) O.Out();
  }
  void std_registerINC(const Byte& reg)
  {
    if (reg == RA) A.Value++;
    if (reg == RB) B.Value++;
    if (reg == RC) C.Value++;
    if (reg == RD) D.Value++;
    if (reg == RE) E.Value++;
    if (reg == RO) O.Value++;
  }
  void std_registerDEC(const Byte& reg)
  {
    if (reg == RA) A.Value--;
    if (reg == RB) B.Value--;
    if (reg == RC) C.Value--;
    if (reg == RD) D.Value--;
    if (reg == RE) E.Value--;
    if (reg == RO) O.Value--;
  }
  Word std_registerGetValue(const Byte& reg)
  {
    if (reg == RA) return A.Value;
    else if (reg == RB) return B.Value;
    else if (reg == RC) return C.Value;
    else if (reg == RD) return D.Value;
    else if (reg == RE) return E.Value;
    else if (reg == RO) return O.Value;
    else return 0xAD9C;
  }



  //Setting Functions
  void SetLog(const LogLevel& level)
    {
        m_level = level;
    }
  void SetDelay(const uint64_t& useconds)
    {
        m_delayUSeconds = useconds;
    }
  void SetMem(const Word& index, const Byte& w)
    {
        
    }

  //Assembler
  void Assemble(const char* ocpath, const Byte& config = 2, const char* xpath = "__UNDETERMINED__")
  {
    loadedProgram = ocpath;
    //Config flags
    bool printTokenizedLines = config & 0b001;
    bool printCompiledBytes = config & 0b0010;
    bool saveCompiledCode = config & 0b0100;
    bool loadCompiledCode = config & 0b1000;
    bool compilationSucceeded = true;
    if (saveCompiledCode && xpath == "__UNDETERMINED__")
    {
      compilationSucceeded = false;
      printd("ERROR: No output path to Assemble");
    }

    String filepath = String("C:/Dev/Macaw16/HD/") + String(ocpath);
    String outpath = String("C:/Dev/Macaw16/HD/") + String(xpath);
    std::vector<String> lines = lineReader.getLines(filepath.c_str());

    //std::vector<std::vector<String>> tokenLines = lineReader.tokenize(lines, ' ');
    //std::vector<std::vector<String>> dataTokens;
    //std::vector<std::vector<String>> codeTokens;
    TokenizedOCVectors tokens = lineReader.tokenize(lines);
    //print tokenized lines
    if (printTokenizedLines) // (print array) USE THIS TO VISUALIZE AND CREATE THE DATA SECTION
    {
      std::cout << "DATA:[" << '\n';
      for (std::vector<String> arr : tokens.data)
      {
        std::cout << "  [";
        for (String token : arr)
        {
          std::cout << ' ' << '\'' << token << '\'';
        }
        std::cout << "]" << '\n';
      }
      std::cout << "]" << '\n';
      std::cout << "CODE:[" << '\n';
      for (std::vector<String> arr : tokens.code)
      {
        std::cout << "  [";
        for (String token : arr)
        {
          std::cout << ' ' << '\'' << token << '\'';
        }
        std::cout << "]" << '\n';
      }
      std::cout << "]" << '\n';
    }

    //COMPILE DATA
    for (int index = 0; index < tokens.data.size(); index++)//Need to write PS PB and PW commands!!!
    {
      int lineNumber = index + 1;
      std::vector<String> tokenizedDataLine = tokens.data[index];
      String command = toUpper(tokenizedDataLine[0]);
      if (command == "PS")
      {
        if (tokenizedDataLine.size() > 2)
        {
          LogCompilationError("Data", lineNumber, "Too much operands", compilationSucceeded);
          continue;
        }
        if (tokenizedDataLine.size() < 2)
        {
          LogCompilationError("Data", lineNumber, "Too few operands", compilationSucceeded);
          continue;
        }
        //MANIPULATE STRING
        //pushstring
      }
      else if (command == "PB")
      {
        if (tokenizedDataLine.size() > 2)
        {
          LogCompilationError("Data", lineNumber, "Too much operands", compilationSucceeded);
          continue;
        }
        if (tokenizedDataLine.size() < 2)
        {
          LogCompilationError("Data", lineNumber, "Too few operands", compilationSucceeded);
          continue;
        }
        //checkbyte
        //pushbyte
      }
      else if (command == "PW")
      {
        if (tokenizedDataLine.size() > 2)
        {
          LogCompilationError("Data", lineNumber, "Too much operands", compilationSucceeded);
          continue;
        }
        if (tokenizedDataLine.size() < 2)
        {
          LogCompilationError("Data", lineNumber, "Too few operands", compilationSucceeded);
          continue;
        }
        //checkword
        //pushword
      }
      else
      {
        String msg = "Unknown command \"" + command + "\"";
        LogCompilationError("Data", lineNumber, msg, compilationSucceeded);
      }
    }
    //COMPILE CODE
    std::vector<Byte> buffer;
    buffer.push_back(1);// x filetype is 1
    //NEED TO ADD DATA SECTION AND CODE SECTION AND DINAMYZE THE NEXT 2 BYTES TO THE BEGGINING
    //OF THE PROGRAM
    buffer.push_back(3);
    buffer.push_back(0);
    //push instructions to the buffer
    for (int i = 0; i < tokens.code.size(); i++)
    {
      std::vector<String> line = tokens.code[i];
      const int lineNumber = i + 1;
      const bool instructionExists = Opcode.count(line[0]) > 0 ? true : false;
      Byte Ins;
      if (instructionExists)
      {
        Ins = Opcode.at(line[0]);
      }
      else
      {
        Ins = 0;
        String msg = "Unknown command \"" + line[0] + "\"";
        LogCompilationError("Code", lineNumber, msg, compilationSucceeded);
        continue;
      }
      buffer.push_back(Ins);
      //Operands 
      if (InsTypes[Ins] == P)
      {
        if (line.size() > 1)
        {
          LogCompilationError("Code", lineNumber, "Too much operands", compilationSucceeded);
          continue;
        }
        if (line.size() < 1)
        {
          LogCompilationError("Code", lineNumber, "Too few operands", compilationSucceeded);
          continue;
        }
      }
      if (InsTypes[Ins] == IB ||
        InsTypes[Ins] == IR ||
        InsTypes[Ins] == IW ||
        InsTypes[Ins] == IL)
      {
        if (line.size() > 2)
        {
          LogCompilationError("Code", lineNumber, "Too much operands", compilationSucceeded);
          continue;
        }
        if (line.size() < 2)
        {
          LogCompilationError("Code", lineNumber, "Too few operands", compilationSucceeded);
          continue;
        }
      }
      if (InsTypes[Ins] == IBB ||
        InsTypes[Ins] == IRR ||
        InsTypes[Ins] == IBW ||
        InsTypes[Ins] == IRW ||
        InsTypes[Ins] == IWB ||
        InsTypes[Ins] == IWR)
      {
        if (line.size() > 3)
        {
          LogCompilationError("Code", lineNumber, "Too much operands", compilationSucceeded);
          continue;
        }
        if (line.size() < 3)
        {
          LogCompilationError("Code", lineNumber, "Too few operands", compilationSucceeded);
          continue;
        }
      }
      if (
        InsTypes[Ins] == IRRR ||
        InsTypes[Ins] == IBBB)
      {
        if (line.size() > 4)
        {
          LogCompilationError("Code", lineNumber, "Too much operands", compilationSucceeded);
          continue;
        }
        if (line.size() < 4)
        {
          LogCompilationError("Code", lineNumber, "Too few operands", compilationSucceeded);
          continue;
        }
      }

      // push operands
      if (InsTypes[Ins] == IB)
      {
        if (!checkByte(line[1], lineNumber, "Code")) compilationSucceeded = false;
        buffer.push_back(toNumber(line[1]));
      }
      else if (InsTypes[Ins] == IR)
      {
        String regString = line[1];
        if (RegMap.count(regString) > 0)
        {
          buffer.push_back(RegMap[regString]);
        }
        else
        {
          String msg = "Unknown Register \"" + regString + "\"";
          LogCompilationError("Code", lineNumber, msg, compilationSucceeded);
        }
      }
      else if (InsTypes[Ins] == IW)
      {
        if (!checkWord(line[1], lineNumber, "Code")) compilationSucceeded = false;
        Word word = toNumber(line[1]);
        Byte upper = word >> 8;
        buffer.push_back(word - 256 * upper);
        buffer.push_back(upper);
      }
      else if (InsTypes[Ins] == IL)
      {
        if (!checkWord(line[1], lineNumber, "Code")) compilationSucceeded = false;
        if (toNumber(line[1]) < 1 || toNumber(line[1]) > tokens.code.size())
        {
          LogCompilationError("Code", lineNumber, "Invalid line Number", compilationSucceeded);
          return;

        }
        Word lineAddress = toNumber(line[1]);
        Word address = 0;
        for (Byte index = 0; index < lineAddress - 1; index++)
        {
          Instruction lineIns = Opcode.at(tokens.code[index][0]);
          address += (InstructionBytes[lineIns] + 1);
        }
        Byte upper = address >> 8;
        buffer.push_back(address - 256 * upper);
        buffer.push_back(upper);
      }
      else if (InsTypes[Ins] == IBB)
      {
        if (!checkByte(line[1], lineNumber, "Code")) compilationSucceeded = false;
        if (!checkByte(line[2], lineNumber, "Code")) compilationSucceeded = false;
        buffer.push_back(toNumber(line[1]));
        buffer.push_back(toNumber(line[2]));
      }
      else if (InsTypes[Ins] == IRR)
      {
        String regString = line[1];
        if (RegMap.count(regString) > 0)
        {
          buffer.push_back(RegMap.at(regString));
        }
        else
        {
          String msg = "Unknown Register \"" + regString + "\"";
          LogCompilationError("Code", lineNumber, msg, compilationSucceeded);
        }
        regString = line[2];
        if (RegMap.count(regString) > 0)
        {
          buffer.push_back(RegMap.at(regString));
        }
        else
        {
          String msg = "Unknown Register \"" + regString + "\"";
          LogCompilationError("Code", lineNumber, msg, compilationSucceeded);
        }
      }
      else if (InsTypes[Ins] == IBW)
      {
        if (!checkByte(line[1], lineNumber, "Code")) compilationSucceeded = false;
        if (!checkWord(line[2], lineNumber, "Code")) compilationSucceeded = false;
        buffer.push_back(toNumber(line[1]));
        Word word = toNumber(line[2]);
        Byte upper = word >> 8;
        buffer.push_back(word - 256 * upper);
        buffer.push_back(upper);
      }
      else if (InsTypes[Ins] == IRW)
      {
        String regString = line[1];
        if (RegMap.count(regString) > 0)
        {
          buffer.push_back(RegMap.at(regString));
        }
        else
        {
          String msg = "Unknown Register \"" + regString + "\"";
          LogCompilationError("Code", lineNumber, msg, compilationSucceeded);
        }
        if (!checkWord(line[2], lineNumber, "Code"))compilationSucceeded = false;
        Word word = toNumber(line[2]);
        Byte upper = word >> 8;
        buffer.push_back(word - 256 * upper);
        buffer.push_back(upper);
      }
      else if (InsTypes[Ins] == IWB)
      {
        if (!checkWord(line[1], lineNumber, "Code")) compilationSucceeded = false;
        if (!checkByte(line[2], lineNumber, "Code")) compilationSucceeded = false;
        Word word = toNumber(line[1]);
        Byte upper = word >> 8;
        buffer.push_back(word - 256 * upper);
        buffer.push_back(upper);
        buffer.push_back(toNumber(line[2]));
      }
      else if (InsTypes[Ins] == IWR)
      {
        if (!checkWord(line[1], lineNumber, "Code")) compilationSucceeded = false;
        Word word = toNumber(line[1]);
        Byte upper = word >> 8;
        buffer.push_back(word - 256 * upper);
        buffer.push_back(upper);
        //check register
        String regString = line[2];
        if (RegMap.count(regString) > 0)
        {
          buffer.push_back(RegMap.at(regString));
        }
        else
        {
          String msg = "Unknown Register \"" + regString + "\"";
          LogCompilationError("Code", lineNumber, msg, compilationSucceeded);
        }

      }
      else if (InsTypes[Ins] == IBBB)
      {
        if (!checkByte(line[1], lineNumber, "Code")) compilationSucceeded = false;
        if (!checkByte(line[2], lineNumber, "Code")) compilationSucceeded = false;
        if (!checkByte(line[3], lineNumber, "Code")) compilationSucceeded = false;
        buffer.push_back(toNumber(line[1]));
        buffer.push_back(toNumber(line[2]));
        buffer.push_back(toNumber(line[3]));
      }
      else if (InsTypes[Ins] == IRRR)
      {
        String regString = line[1];
        if (RegMap.count(regString) > 0)
        {
          buffer.push_back(RegMap.at(regString));
        }
        else
        {
          String msg = "Unknown Register \"" + regString + "\"";
          LogCompilationError("Code", lineNumber, msg, compilationSucceeded);
        }
        regString = line[2];
        if (RegMap.count(regString) > 0)
        {
          buffer.push_back(RegMap.at(regString));
        }
        else
        {
          String msg = "Unknown Register \"" + regString + "\"";
          LogCompilationError("Code", lineNumber, msg, compilationSucceeded);
        }
        regString = line[3];
        if (RegMap.count(regString) > 0)
        {
          buffer.push_back(RegMap.at(regString));
        }
        else
        {
          String msg = "Unknown Register \"" + regString + "\"";
          LogCompilationError("Code", lineNumber, msg, compilationSucceeded);
        }
      }
    }

    //Print the buffer
    if (printCompiledBytes && compilationSucceeded)
    {
      printsl("[");
      for (Byte el : buffer)
      {
        std::cout << " " << (int)el;
      }
      printd("]");
    }
    //Save the buffer to a file
    if (saveCompiledCode && compilationSucceeded)
    {
      writer.WriteByteArray(buffer, outpath.c_str());

    }
    //Load the buffer in the cpu
    if (loadCompiledCode && compilationSucceeded)
    {
      LoadXByteArray(buffer);
    }
    if (!compilationSucceeded)
    {
      std::cout << "Compilation Failed!" << '\n';
      halt = true;
    }
  }
private:
  void LogCompilationError(const String& section, const int& lineNumber, const String& message, bool& success)
  {
    std::cout << "[" << section << ":Line " << lineNumber << "] " << message << '\n';
    success = false;
  }
  void Log(const String& message, const LogLevel& level)
  {
    if (m_level != level) return;
    printd(message);
  }
  void LoadXByteArray(std::vector<Byte> arr)
  {
    Byte programBegin = (arr[2] << 8) + arr[1];
    // getData
    for (Word i = 3; i < programBegin; i++)
    {
      PushByteRAM(arr[i], 1);
    }
    // getProgram
    for (Word i = programBegin; i < arr.size(); i++)
    {
      //ram.SetByte(offset, bytes[i]);
      //offset++;
      PushByteRAM(arr[i], 0);

    }
    unHalt();
  }
  void unHalt()
  {
    halt = false;
  }

};
