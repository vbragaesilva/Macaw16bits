# V16

## 1) Getting started

  ### 1.1) Main.cpp

~~~C
	#include "CPU.h"
	CPU cpu(delayUSeconds, LogLevel);
~~~

### 1.2) The .x file
* 1st byte: filetype  
* 2nd and 3rd bytes: instructions beggining(b)
* 4th to (b)th byte: data (in Words)
* bth to last byte: program instructions

### 1.3) Programming and running the computer
* To program Macaw16 with a executable .x file, use the following function:
~~~javascript
  cpu.LoadXFile("path/inside/HD/folder/to/the/file.x");
~~~

* To program it with a assembly .oc file:
~~~javascript
  cpu.Assemble("path/to/file.oc", config*, "path/to/output/file.x"*);
~~~
  * &ast; optional <br>
  * config can be expressed using binary operations with the following flags:
    * CPU::PRINT_LINES: print the .oc lines tokenized for debug;
    * CPU::PRINT_BYTES: print the final program compiled into bytes;
    * CPU::SAVE_BYTES: save the compiled bytes to the output file;
    * CPU::LOAD_BYTES: load the compiled bytes to the memory.
* To program it manually, use the corresponding function:
~~~javascript
  cpu.PushInstruction(CPU::NOP);
  cpu.PushInstructionByte(CPU::ADD, CPU::RA);
  cpu.PushInstructionByteByte(CPU::LDR, CPU::RA, CPU::RB);
  cpu.PushInstructionWord(CPU::INS, DADA);
  cpu.PushInstructionByteByteByte(CPU::ADDR, CPU::RA, CPU::RB, CPU::RC);
  cpu.PushInstructionByteWord(CPU::LDI, CPU::RA, DADA);
  cpu.PushInstructionWordByte(CPU::ADDI, DADA, CPU::RA);
~~~
  * Registers are treated as Bytes
  * Lines are treated as Words

## 2) Instructions

### 2.1) Instruction layout

* Pure (P)
  * Size: 1 byte
  * Format: II -> (instruction)

* Instruction-Byte (IB)
  * Size: 2 bytes
  * Format: II DA

* Instruction-Byte-Byte (IBB)
  * Size: 3 bytes
  * Format: II DA DA

* Instruction-Word (IW)
  * Size: 3 bytes
  * Format: II DADA

* Instruction-Byte-Byte-Byte (IBBB)
  * Size: 4 bytes
  * Format: II DA DA DA

* Intruction-Byte-Word (IBW)
  * Size: 4 bytes
  * Format: II DA DADA

* Instruction-Word-Byte (IWB)
  * Size: 4 bytes
  * Format: II DADA DA

* Instruction-Register (IR)
  * Size: 2 bytes
  * Format: II RX

* Instruction-Register-Register (IRR)
  * Size: 3 bytes
  * Format: II RX RX

* Instruction-Register-Register-Register (IRRR)
  * Size: 4 bytes
  * Format: II RX RX RX

* Instruction-Register-Word (IRW)
  * Size: 4 bytes
  * Format: II RX DADA

* Instruction-Word-Register (IWR)
  * Size: 4 bytes
  * Format: II DADA RX

* Instruction-Line (IL)
  * Size: 3 bytes
  * Format: II LLLL



### 2.2) Instruction set
* NOP
  * Number: 00
  * Name: No Operation
  * Type: P
  * Format: II
  * Description: Does no operation for 1 cycle.
  
* LARB
  * Number: 01
  * Name: Load Address Register Byte
  * Type: IRR
  * Format: II RX RY
  * Description: Loads RX with the Byte on the address given by RY

* LARW
  * Number: 02
  * Name: Load Address Register Word
  * Type: IRR
  * Format: II RX RY
  * Description: Loads RX with the Word on the address given by RY

* LDI
  * Number: 03
  * Name: Load Immediate
  * Type: IRW
  * Format: II RX FFFF
  * Description: Load Register RX with value FFFF.

* LAB
  * Number: 04
  * Name: Load Address Byte
  * Type: IRW
  * Format: II RX ADAD
  * Description: Load register RX with byte on address ADAD.

* LAW
  * Number: 05
  * Name: Load Address Word
  * Type: IRW
  * Format: II RX ADAD
  * Description: Load register RX with word on address ADAD.

* LDR
  * Number: 06
  * Name: Load from Register
  * Type: IRR
  * Format: II RX RY
  * Description: Load Register RX with value on Register RY.

* SARL
  * Number: 07
  * Name: Store Address Register Lower
  * Type: IRR
  * Format: II RX RY
  * Description: Stores RX's lower Byte on the address given by RY

 * SARU
  * Number: 08
  * Name: Store Address Register Upper
  * Type: IRR
  * Format: II RX RY
  * Description: Stores RX's upper Byte on the address given by RY

* SARW
  * Number: 09
  * Name: Store Address Register Word
  * Type: IRR
  * Format: II RX RY
  * Description: Stores RX Value on the address given by RY

* STL
  * Number: 0A (10)
  * Name: Store Lower
  * Type: IRW
  * Format: II RX ADAD
  * Description: Stores lower byte of Reg RX in address ADAD.

* STU
  * Number: 0B (11)
  * Name: Store Upper
  * Type: IRW
  * Format: II RX ADAD
  * Description: Stores upper byte of Reg RX in address ADAD.

* STW
  * Number: 0C (12)
  * Name: Store Word
  * Type: IRW
  * Format: II RX ADAD
  * Description: Stores value of Reg RX in address ADAD.

* STI
  * Number: 0D (13)
  * Name: Store Immediate
  * Type: IBW
  * Format: II FF ADAD
  * Description: Stores value FF in address ADAD.

* JMP
  * Number: 0E (14)
  * Name: Jump
  * Type: IL
  * Format: II ADAD
  * Description: Jumps progam counter to address ADAD.

* JC
  * Number: 0F (15)
  * Name: Jump if Carry
  * Type: IL
  * Format: II ADAD
  * Description: Jumps to address ADAD if carry flag is ON.

* JNC
  * Number: 10 (16)
  * Name: Jump if Not Carry
  * Type: IL
  * Format: II ADAD
  * Description: Jumps to address ADAD(counting from the beggining of program instructions)
    if carry flag is OFF.

* JZ
  * Number: 11 (17)
  * Name: Jump if Zero
  * Type: IL
  * Format: II ADAD
  * Description: Jumps to address ADAD if result of previous arithmetic operation is zero (Zero flag is ON).

* JNZ
  * Number: 12 (18)
  * Name: Jump if Not Zero
  * Type: IL
  * Format: II ADAD
  * Description: Jumps to address ADAD if result of previous arithmetic operation is not zero (Zero flag is OFF).

* JN
  * Number: 13 (19)
  * Name: Jump if Negative
  * Type: IL
  * Format: II ADAD
  * Description: Jumps to address ADAD if result of previous arithmetic operation is negative (Negative flag is ON).

* JNN
  * Number: 14 (20)
  * Name: Jump if Not Negative
  * Type: IL
  * Format: II ADAD
  * Description: Jumps to address ADAD if result of previous arithmetic operation is not negative (Negative flag is OFF).

* ADD
  * Number: 15 (21)
  * Name: Add
  * Type: IR
  * Format: II RX
  * Description: Adds the values of RA and RB and stores the result in register RX.

* ADDA
  * Number: 16 (22)
  * Name: Add from address
  * Type: IWR
  * Format: II ADAD RX
  * Description: Adds the value of RA and the value on the address ADAD and stores the result in register RX (Changes the value of RB).

* ADDI
  * Number: 17 (23)
  * Name: Add Immediate
  * Type: IWR
  * Format: II FFFF RX
  * Description: Adds the value of RA and the value FFFF and stores the result in register RX (Changes the value of RB).

* ADDR
  * Number: 18 (24)
  * Name: Add Register
  * Type: IRRR
  * Format: II RX RY RZ
  * Description: Adds the values of registers RX and RY and stores the result in register RZ (Changes the values of RA and RB).

  * SUB
  * Number: 19 (25)
  * Name: Subtract
  * Type: IR
  * Format: II RX
  * Description: Subtracts the value of RB from RA and stores the result in register RX.

* SUBA
  * Number: 1A (26)
  * Name: Subtract from address
  * Type: IWR
  * Format: II ADAD RX
  * Description: Subtracts the value on the address ADAD from RA and stores the result in register RX (Changes the value of RB).

* SUBI
  * Number: 1B (27)
  * Name: Subtract Immediate
  * Type: IWR
  * Format: II FFFF RX
  * Description: Subtracts the value FFFF from RA and stores the result in register RX (Changes the value of RB).

* SUBR
  * Number: 1C (28)
  * Name: Subtract Register
  * Type: IRRR
  * Format: II RX RY RZ
  * Description: Subtracts the value of register RY from RX and stores the result in register RZ (Changes the values of RA and RB).

* MULT
  * Number: 1D (29)
  * Name: Multiply
  * Type: IR
  * Format: II RX
  * Description: Multiply the values of RA and RB and stores the result in register RX.

* DIV
  * Number: 1E (30)
  * Name: Divide
  * Type: IR
  * Format: II RX
  * Description: Divide the values of RA and RB and stores the result in register RX.

* INC
  * Number: 1F (31)
  * Name: Increment
  * Type: IR
  * Format: II RX
  * Description: Increments the value of RX.

* DEC
  * Number: 20 (32)
  * Name: Decrement
  * Type: IR
  * Format: II RX
  * Description: Decrements the value of RX.

* PUSH
  * Number: 21 (33)
  * Name: Push Stack
  * Type: IR
  * Format: II RX
  * Description: Pushes the value of Register RX into the stack.

* POP
  * Number: 22 (34)
  * Name: Pop Stack
  * Type: IR
  * Format: II RX
  * Description: Pops the stack into Register RX.

* PRNT
  * Number: FD (253)
  * Name: Print
  * Type: P
  * Format: II
  * Description: Prints the Lower Byte of RO as (char).

* OUT
  * Number: FE (254)
  * Name: Output
  * Type: P
  * Format: II
  * Description: Outputs the value of RO with fancy arrow.

* HLT
  * Number: FF (255)
  * Name: Halt
  * Type: P
  * Format: II
  * Description: Halts the cpu.

### 2.3) Registering a New Instruction

* I) (ENUM): Add the opcode to CPU::Instruction enum
* III) (INSTRUCTIONBYTES): Add the number of data bytes to Init() -> Init Instructions
* IV) (INSTYPES): Add it to the InsTypes
* II) (OPCODE): Add the instruction to CPU::Opcode map
* V) (EXEC): Add the Insruction and its micro-instructions to the Exec() Function
* VI) (DOCS): Add it to this Documentation item 2.2 :)

## 3) Registers

### 3.1) Adding new Registers
* I) Add the Reg to the public cpu attributes. 
* II) Add it to the Init components section, connecting it to the bus;
* III) Add it to the Registers enum;
* IV) Add it to the std_registerIn, std_registerOut, std_RegisterINC, std_RegisterDEC, std_registerGetValue funtions;
* V) Add it to the Init Register Map section

## 4) Programming the computer

## 5) RAM

The RAM has 3 banks:
- 0: Instructions
- 1: Data
- 2: Stack
- 3: ???



