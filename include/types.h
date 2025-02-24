#pragma once
#include <cstdint>

namespace nes {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

class IAddressable;
class CPU;

// Interface for memory-mapped components
class Addressable {
public:
  virtual ~Addressable() = default;
  virtual u8 read(u16 address) const = 0;
  virtual void write(u16 address, u8 value) = 0;
  virtual bool handles_address(u16 address) const = 0;
};

using InstructionHandler = void (CPU::*)();

struct Instruction {
  InstructionHandler handler;
  u8 cycles;
  const char *name;
};

enum class Opcode : u8 {
  // LDA
  LDA_IM = 0xA9,   // LDA Immediate
  LDA_ABS = 0xAD,  // LDA Absolute
  LDA_XABS = 0xBD, // LDA X-Index Absolute
  LDA_YABS = 0xB9, // LDA Y-Index Absolute
  LDA_ZP = 0xA5,   // LDA Zero Page
  LDA_XZP = 0xB5,  // LDA X-Index Zero Page
  LDA_XZPI = 0xA1, // LDA X-Index Zero Page Indirect
  LDA_YZPI = 0xB1, // LDA Y-Index Zero Page Indirect
  // LDX
  LDX_IM = 0xA2,   // LDX Immediate
  LDX_ABS = 0xAE,  // LDX Absolute
  LDX_YABS = 0xBE, // LDX Y-Index Absolute
  LDX_ZP = 0xA6,   // LDX Zero Page
  LDX_YZP = 0xB6,  // LDX Y-Index Zero Page
  // LDY
  LDY_IM = 0xA0,   // LDY Immediate
  LDY_ABS = 0xAC,  // LDY Absolute
  LDY_XABS = 0xBC, // LDY X-Index Absolute
  LDY_ZP = 0xA4,   // LDY Zero Page
  LDY_XZP = 0xB4,  // LDY X-Index Zero Page
  // STA
  STA_ABS = 0x8D,  // STA Absolute
  STA_XABS = 0x9D, // STA X-Index Absolute
  STA_YABS = 0x99, // STA Y-Index Absolute
  STA_ZP = 0x85,   // STA Zero Page
  STA_XZP = 0x95,  // STA Zero Page X
  STA_XZPI = 0x81, // STA Zero Page X Indirect
  STA_YZPI = 0x91, // STA Zero Page Y Indirect
  // STX
  STX_ABS = 0x8E, // STX Absolute
  STX_ZP = 0x86,  // STX Zero Page
  STX_YZP = 0x96, // STX Zero Page Y-Index
  // STY
  STY_ABS = 0x8C, // STY Absolute
  STY_ZP = 0x84,  // STY Zero Page
  STY_XZP = 0x94, // STY Zero Page X-Index
  // Transfer
  TAX = 0xAA, // Transfer A to X
  TXA = 0x8A, // Transfer X to A
  TAY = 0xA8, // Transfer A to Y
  TSX = 0xBA, // Transfer Stack Pointer to Index X
  TXS = 0x9A, // Transfer Index X to Stack Pointer
  TYA = 0x98, // Transfer Index X to A
  // Stack
  PHA = 0x48, // Push A to the Stack
  PHP = 0x08, // Push Status to the Stack
  PLA = 0x68, // Pull A from the Stack
  PLP = 0x28, // Pull Status from the Stack
  // ASL
  ASL_ACC = 0x0A,  // Arithmetic Shift Left with A
  ASL_ABS = 0x0E,  // Arithmetic Shift Left Absolute
  ASL_XABS = 0x1E, // Arithmetic Shift Left Absolute X-Index
  ASL_ZP = 0x06,   // Arithmetic Shift Left Zero Page
  ASL_XZP = 0x16,  // Arithmetic Shift Left Zero Page X-Index
};

enum class Flag : u8 {
  CARRY = 0x01,
  ZERO = 0x02,
  INTERRUPT_DISABLE = 0x04,
  DECIMAL = 0x08,
  BREAK = 0x10,
  UNUSED = 0x20,
  OVERFLOW_ = 0x40,
  NEGATIVE = 0x80
};

} // namespace nes
