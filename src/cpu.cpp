#include "../include/cpu.h"

#include <string>

#include "types.h"

namespace nes {

CPU::CPU(Bus &bus_ref)
  : _bus(bus_ref) {
  reset();
  _instruction_table = {
    // LDA
    {(u8)Opcode::LDA_IMM, {.addressed_op = &CPU::op_lda, .mode = &CPU::immediate, .cycles = 2, .name = "LDA"}},
    {(u8)Opcode::LDA_ZPG, {.addressed_op = &CPU::op_lda, .mode = &CPU::zero_page, .cycles = 3, .name = "LDA"}},
    {(u8)Opcode::LDA_ABS, {.addressed_op = &CPU::op_lda, .mode = &CPU::absolute, .cycles = 4, .name = "LDA"}},
    {(u8)Opcode::LDA_ABX,
     {.addressed_op = &CPU::op_lda, .mode = &CPU::absolute_x, .cycles = 4, .name = "LDA", .is_extra_cycle = true}},
    {(u8)Opcode::LDA_ABY,
     {.addressed_op = &CPU::op_lda, .mode = &CPU::absolute_y, .cycles = 4, .name = "LDA", .is_extra_cycle = true}},
    {(u8)Opcode::LDA_ZPX, {.addressed_op = &CPU::op_lda, .mode = &CPU::zero_page_x, .cycles = 4, .name = "LDA"}},
    {(u8)Opcode::LDA_IZX, {.addressed_op = &CPU::op_lda, .mode = &CPU::indirect_x, .cycles = 6, .name = "LDA"}},
    {(u8)Opcode::LDA_IZY,
     {.addressed_op = &CPU::op_lda, .mode = &CPU::indirect_y, .cycles = 5, .name = "LDA", .is_extra_cycle = true}},

    // LDX
    {(u8)Opcode::LDX_IMM, {.addressed_op = &CPU::op_ldx, .mode = &CPU::immediate, .cycles = 2, .name = "LDX"}},
    {(u8)Opcode::LDX_ABS, {.addressed_op = &CPU::op_ldx, .mode = &CPU::absolute, .cycles = 4, .name = "LDX"}},
    {(u8)Opcode::LDX_ABY,
     {.addressed_op = &CPU::op_ldx, .mode = &CPU::absolute_y, .cycles = 4, .name = "LDX", .is_extra_cycle = true}},
    {(u8)Opcode::LDX_ZPG, {.addressed_op = &CPU::op_ldx, .mode = &CPU::zero_page, .cycles = 3, .name = "LDX"}},
    {(u8)Opcode::LDX_ZPY, {.addressed_op = &CPU::op_ldx, .mode = &CPU::zero_page_y, .cycles = 4, .name = "LDX"}},

    // LDY
    {(u8)Opcode::LDY_IMM, {.addressed_op = &CPU::op_ldy, .mode = &CPU::immediate, .cycles = 2, .name = "LDY"}},
    {(u8)Opcode::LDY_ABS, {.addressed_op = &CPU::op_ldy, .mode = &CPU::absolute, .cycles = 4, .name = "LDY"}},
    {(u8)Opcode::LDY_ABX,
     {.addressed_op = &CPU::op_ldy, .mode = &CPU::absolute_x, .cycles = 4, .name = "LDY", .is_extra_cycle = true}},
    {(u8)Opcode::LDY_ZPG, {.addressed_op = &CPU::op_ldy, .mode = &CPU::zero_page, .cycles = 3, .name = "LDY"}},
    {(u8)Opcode::LDY_ZPX, {.addressed_op = &CPU::op_ldy, .mode = &CPU::zero_page_x, .cycles = 4, .name = "LDY"}},

    // STA
    {(u8)Opcode::STA_ABS, {.addressed_op = &CPU::op_sta, .mode = &CPU::absolute, .cycles = 4, .name = "STA"}},
    {(u8)Opcode::STA_ABX, {.addressed_op = &CPU::op_sta, .mode = &CPU::absolute_x, .cycles = 5, .name = "STA"}},
    {(u8)Opcode::STA_ABY, {.addressed_op = &CPU::op_sta, .mode = &CPU::absolute_y, .cycles = 5, .name = "STA"}},
    {(u8)Opcode::STA_ZPG, {.addressed_op = &CPU::op_sta, .mode = &CPU::zero_page, .cycles = 3, .name = "STA"}},
    {(u8)Opcode::STA_ZPX, {.addressed_op = &CPU::op_sta, .mode = &CPU::zero_page_x, .cycles = 4, .name = "STA"}},
    {(u8)Opcode::STA_IZX, {.addressed_op = &CPU::op_sta, .mode = &CPU::indirect_x, .cycles = 6, .name = "STA"}},
    {(u8)Opcode::STA_IZY, {.addressed_op = &CPU::op_sta, .mode = &CPU::indirect_y, .cycles = 6, .name = "STA"}},

    // STX
    {(u8)Opcode::STX_ABS, {.addressed_op = &CPU::op_stx, .mode = &CPU::absolute, .cycles = 4, .name = "STX"}},
    {(u8)Opcode::STX_ZPG, {.addressed_op = &CPU::op_stx, .mode = &CPU::zero_page, .cycles = 3, .name = "STX"}},
    {(u8)Opcode::STX_ZPY, {.addressed_op = &CPU::op_stx, .mode = &CPU::zero_page_y, .cycles = 4, .name = "STX"}},

    // STY
    {(u8)Opcode::STY_ABS, {.addressed_op = &CPU::op_sty, .mode = &CPU::absolute, .cycles = 4, .name = "STY"}},
    {(u8)Opcode::STY_ZPG, {.addressed_op = &CPU::op_sty, .mode = &CPU::zero_page, .cycles = 3, .name = "STY"}},
    {(u8)Opcode::STY_ZPX, {.addressed_op = &CPU::op_sty, .mode = &CPU::zero_page_x, .cycles = 4, .name = "STY"}},

    // Transfer operations (implied addressing)
    {(u8)Opcode::TAX_IMP,
     {.implied_op = &CPU::op_tax, .mode = nullptr, .cycles = 2, .name = "TAX", .is_implied = true}},
    {(u8)Opcode::TAY_IMP,
     {.implied_op = &CPU::op_tay, .mode = nullptr, .cycles = 2, .name = "TAY", .is_implied = true}},
    {(u8)Opcode::TSX_IMP,
     {.implied_op = &CPU::op_tsx, .mode = nullptr, .cycles = 2, .name = "TSX", .is_implied = true}},
    {(u8)Opcode::TYA_IMP,
     {.implied_op = &CPU::op_tya, .mode = nullptr, .cycles = 2, .name = "TYA", .is_implied = true}},
    {(u8)Opcode::TXS_IMP,
     {.implied_op = &CPU::op_txs, .mode = nullptr, .cycles = 2, .name = "TXS", .is_implied = true}},
    {(u8)Opcode::TXA_IMP,
     {.implied_op = &CPU::op_txa, .mode = nullptr, .cycles = 2, .name = "TXA", .is_implied = true}},

    // Stack operations (implied addressing)
    {(u8)Opcode::PHA_IMP,
     {.implied_op = &CPU::op_pha, .mode = nullptr, .cycles = 3, .name = "PHA", .is_implied = true}},
    {(u8)Opcode::PLA_IMP,
     {.implied_op = &CPU::op_pla, .mode = nullptr, .cycles = 4, .name = "PLA", .is_implied = true}},
    {(u8)Opcode::PLP_IMP,
     {.implied_op = &CPU::op_plp, .mode = nullptr, .cycles = 4, .name = "PLP", .is_implied = true}},
    {(u8)Opcode::PHP_IMP,
     {.implied_op = &CPU::op_php, .mode = nullptr, .cycles = 3, .name = "PHP", .is_implied = true}},

    // ASL
    {(u8)Opcode::ASL_ACC,
     {.implied_op = &CPU::op_asl_acc, .mode = nullptr, .cycles = 2, .name = "ASL", .is_implied = true}},
    {(u8)Opcode::ASL_ABS, {.addressed_op = &CPU::op_asl, .mode = &CPU::absolute, .cycles = 6, .name = "ASL"}},
    {(u8)Opcode::ASL_ABX, {.addressed_op = &CPU::op_asl, .mode = &CPU::absolute_x, .cycles = 7, .name = "ASL"}},
    {(u8)Opcode::ASL_ZPG, {.addressed_op = &CPU::op_asl, .mode = &CPU::zero_page, .cycles = 5, .name = "ASL"}},
    {(u8)Opcode::ASL_ZPX, {.addressed_op = &CPU::op_asl, .mode = &CPU::zero_page_x, .cycles = 6, .name = "ASL"}},

    // LSR
    {(u8)Opcode::LSR_ACC,
     {.implied_op = &CPU::op_lsr_acc, .mode = nullptr, .cycles = 2, .name = "LSR", .is_implied = true}},
    {(u8)Opcode::LSR_ABS, {.addressed_op = &CPU::op_lsr, .mode = &CPU::absolute, .cycles = 6, .name = "LSR"}},
    {(u8)Opcode::LSR_ABX, {.addressed_op = &CPU::op_lsr, .mode = &CPU::absolute_x, .cycles = 7, .name = "LSR"}},
    {(u8)Opcode::LSR_ZPG, {.addressed_op = &CPU::op_lsr, .mode = &CPU::zero_page, .cycles = 5, .name = "LSR"}},
    {(u8)Opcode::LSR_ZPX, {.addressed_op = &CPU::op_lsr, .mode = &CPU::zero_page_x, .cycles = 6, .name = "LSR"}},

    // ROL
    //{(u8)Opcode::ROL_ACC,
    // {.implied_op = &CPU::op_rol_acc, .mode = nullptr, .cycles = 2, .name = "ROL", .is_implied = true}},
    //{(u8)Opcode::ROL_ABS, {.addressed_op = &CPU::op_rol, .mode = &CPU::absolute, .cycles = 6, .name = "ROL"}},
    //{(u8)Opcode::ROL_ABX, {.addressed_op = &CPU::op_rol, .mode = &CPU::absolute_x, .cycles = 7, .name = "ROL"}},
    //{(u8)Opcode::ROL_ZPG, {.addressed_op = &CPU::op_rol, .mode = &CPU::zero_page, .cycles = 5, .name = "ROL"}},
    //{(u8)Opcode::ROL_ZPX, {.addressed_op = &CPU::op_rol, .mode = &CPU::zero_page_x, .cycles = 6, .name = "ROL"}},

    // Flags
    {(u8)Opcode::CLC_IMP, {.implied_op = &CPU::clc, .mode = nullptr, .cycles = 2, .name = "CLC", .is_implied = true}},
    {(u8)Opcode::SEC_IMP, {.implied_op = &CPU::sec, .mode = nullptr, .cycles = 2, .name = "SEC", .is_implied = true}}};
}

void CPU::clock() {
  if (_cycles == 0) {
    u8 opcode = read_byte(_PC++);
    set_flag(Flag::UNUSED, true);

    auto it = _instruction_table.find(opcode);
    if (it == _instruction_table.end()) { throw std::runtime_error("Unknown opcode: " + std::to_string(opcode)); }

    _cycles = it->second.cycles;

    if (it->second.is_implied) {
      // Handle implied addressing operations (no address needed)
      (this->*(it->second.implied_op))();
    } else {
      // Handle operations that require addressing
      auto addr_mode = it->second.mode;

      u16 addr = 0;
      if (addr_mode != nullptr) {
        bool page_crossed = false;
        addr = (this->*addr_mode)(page_crossed);

        if (page_crossed && it->second.is_extra_cycle) { _cycles++; }
      }

      // Execute the addressed operation
      (this->*(it->second.addressed_op))(addr);
    }
  }
  _cycles--;
}

void CPU::reset() {
  _A = 0;
  _X = 0;
  _Y = 0;
  _SP = 0xFF;
  _status = (u8)Flag::UNUSED | (u8)Flag::BREAK;
  _PC = 0xFFFC;
  _cycles = 0;
}

// Memory operations
u8 CPU::read_byte(const u16 address) { return _bus.read(address); }

void CPU::write_byte(const u16 address, const u8 value) { _bus.write(address, value); }

// Getters
u8 CPU::get_accumulator() const { return _A; }
u8 CPU::get_x() const { return _X; }
u8 CPU::get_y() const { return _Y; }
u16 CPU::get_pc() const { return _PC; }
u8 CPU::get_sp() const { return _SP; }
u8 CPU::get_status() const { return _status; }
u8 CPU::get_remaining_cycles() const { return _cycles; }

// Setters
void CPU::set_sp(u8 sp) { _SP = sp; }

// Flag operations
bool CPU::get_flag(Flag flag) const { return (_status & (u8)(flag)) != 0; }

void CPU::set_flag(const Flag flag, const bool value) {
  if (value) {
    _status |= (u8)(flag);
  } else {
    _status &= ~(u8)(flag);
  }
}

void CPU::update_zero_and_negative_flags(u8 value) {
  set_flag(Flag::ZERO, value == 0);
  set_flag(Flag::NEGATIVE, (value & 0x80) != 0);
}

//////////////////////////////////////////////////////////////////////////
// ADDRESSING MODES
//////////////////////////////////////////////////////////////////////////

u16 CPU::immediate(bool &page_crossed) {
  return _PC++;  // Return the PC then increment it
}

u16 CPU::zero_page(bool &page_crossed) {
  return read_byte(_PC++);  // Zero page address is just a single byte
}

u16 CPU::zero_page_x(bool &page_crossed) {
  u8 zp_addr = read_byte(_PC++);
  return (u16)((zp_addr + _X) & 0xFF);  // Wrap around in zero page
}

u16 CPU::zero_page_y(bool &page_crossed) {
  u8 zp_addr = read_byte(_PC++);
  return (u16)((zp_addr + _Y) & 0xFF);  // Wrap around in zero page
}

u16 CPU::absolute(bool &page_crossed) {
  u16 addr_low = read_byte(_PC++);
  u16 addr_high = read_byte(_PC++);
  return (addr_high << 8) | addr_low;
}

u16 CPU::absolute_x(bool &page_crossed) {
  u16 addr_low = read_byte(_PC++);
  u16 addr_high = read_byte(_PC++);
  u16 base_addr = (addr_high << 8) | addr_low;
  u16 final_addr = base_addr + _X;

  page_crossed = ((base_addr & 0xFF00) != (final_addr & 0xFF00));
  return final_addr;
}

u16 CPU::absolute_y(bool &page_crossed) {
  u16 base_addr = absolute(page_crossed);
  u16 final_addr = base_addr + _Y;

  page_crossed = ((base_addr & 0xFF00) != (final_addr & 0xFF00));
  return final_addr;
}

u16 CPU::indirect_x(bool &page_crossed) {
  u8 zp_addr = read_byte(_PC++);
  zp_addr += _X;  // Add X to the zero page address (with wrap)

  // Read two bytes from the computed zero page address
  u16 effective_addr_low = read_byte(zp_addr);
  u16 effective_addr_high = read_byte((u16)((zp_addr + 1) & 0xFF));

  return (effective_addr_high << 8) | effective_addr_low;
}

u16 CPU::indirect_y(bool &page_crossed) {
  u8 zp_addr = read_byte(_PC++);

  u16 effective_addr_low = read_byte(zp_addr);
  u16 effective_addr_high = read_byte(((u16)((zp_addr + 1) & 0xFF)));

  u16 base_addr = (effective_addr_high << 8) | effective_addr_low;
  u16 final_addr = base_addr + _Y;

  page_crossed = ((base_addr & 0xFF00) != (final_addr & 0xFF00));

  return final_addr;
}

//////////////////////////////////////////////////////////////////////////
// ADDRESSED OPERATIONS (operations that need an address)
//////////////////////////////////////////////////////////////////////////

// Load operations
void CPU::op_lda(const u16 addr) {
  _A = read_byte(addr);
  update_zero_and_negative_flags(_A);
}

void CPU::op_ldx(const u16 addr) {
  _X = read_byte(addr);
  update_zero_and_negative_flags(_X);
}

void CPU::op_ldy(const u16 addr) {
  _Y = read_byte(addr);
  update_zero_and_negative_flags(_Y);
}

// Store operations
void CPU::op_sta(const u16 addr) { write_byte(addr, _A); }

void CPU::op_stx(const u16 addr) { write_byte(addr, _X); }

void CPU::op_sty(const u16 addr) { write_byte(addr, _Y); }

// ASL (addressed version)
void CPU::op_asl(const u16 addr) {
  u8 value = read_byte(addr);
  set_flag(Flag::CARRY, (value & 0x80) != 0);
  value <<= 1;
  write_byte(addr, value);
  update_zero_and_negative_flags(value);
}

// LSR (addressed version)
void CPU::op_lsr(const u16 addr) {
  u8 value = read_byte(addr);
  set_flag(Flag::CARRY, (value & 0x01) != 0);
  value >>= 1;
  write_byte(addr, value);
  set_flag(Flag::NEGATIVE, 0);
  set_flag(Flag::ZERO, value == 0);
}

//////////////////////////////////////////////////////////////////////////
// IMPLIED OPERATIONS (operations that don't need an address)
//////////////////////////////////////////////////////////////////////////

// Transfer operations
void CPU::op_tax() {
  _X = _A;
  update_zero_and_negative_flags(_X);
}

void CPU::op_tay() {
  _Y = _A;
  update_zero_and_negative_flags(_Y);
}

void CPU::op_txa() {
  _A = _X;
  update_zero_and_negative_flags(_A);
}

void CPU::op_tsx() {
  _X = _SP;
  update_zero_and_negative_flags(_X);
}

void CPU::op_txs() { _SP = _X; }

void CPU::op_tya() {
  _A = _Y;
  update_zero_and_negative_flags(_A);
}

// Stack operations
void CPU::op_pha() {
  write_byte(0x0100 + _SP, _A);
  _SP--;
}

void CPU::op_php() {
  // When pushing the status register, set bits a4 and 5 (B flag and unused
  // flag)
  write_byte(0x0100 + _SP, _status | 0x30);
  _SP--;
}

void CPU::op_pla() {
  _SP++;
  _A = read_byte(0x0100 + _SP);
  update_zero_and_negative_flags(_A);
}

void CPU::op_plp() {
  _SP++;
  uint8_t pulled_status = read_byte(0x0100 + _SP);
  uint8_t break_flag = _status & 0x10;

  // Set the status register with the pulled value
  // but preserve the Break flag and force Unused flag set
  _status = (pulled_status & ~0x10) | break_flag | 0x20;
}

// ASL and LSR accumulator operations
void CPU::op_asl_acc() {
  set_flag(Flag::CARRY, (_A & 0x80) != 0);
  _A <<= 1;
  update_zero_and_negative_flags(_A);
}

void CPU::op_lsr_acc() {
  set_flag(Flag::CARRY, (_A & 0x01) != 0);
  _A >>= 1;
  set_flag(Flag::NEGATIVE, 0);
  set_flag(Flag::ZERO, _A == 0);
}

// Flag operations
void CPU::sec() { set_flag(Flag::CARRY, true); }
void CPU::clc() { set_flag(Flag::CARRY, false); }

}  // namespace nes
