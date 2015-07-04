#include "CPU.hpp"
#include "Memory.hpp"


// Flag shortcuts
#define z 0b10000000
#define n 0b01000000
#define h 0b00100000
#define c 0b00010000


CPU::CPU():
AF(_AF.word), BC(_BC.word), DE(_DE.word), HL(_HL.word),
A(_AF.hb), F(_AF.lb), B(_BC.hb), C(_BC.lb), D(_DE.hb),
E(_DE.lb), H(_HL.hb), L(_HL.lb)
{
    this->opcodesList[0x00] = &CPU::noop;
    this->opcodesList[0x01] = &CPU::LDBC_nn;
    this->opcodesList[0x02] = &CPU::LDBC_A;
    this->opcodesList[0x03] = &CPU::INC_BC;

    this->reset();
}


CPU::~CPU() { }


void CPU::reset()
{
    AF = 0;
    BC = 0;
    DE = 0;
    HL = 0;
    SP = 0;
    PC = 0;
}


void CPU::oneIteration()
{
    // Get opcode
    Uint16 opcode = this->memory->readWord(this->PC);

    // Execute it
    (this->*(this->opcodesList[opcode]))();

    this->PC &= 0xFFFF;
}

void CPU::noop() {}

void CPU::LDBC_nn()
{
    C = this->memory->readByte(this->PC++);
    B = this->memory->readByte(this->PC++);
    this->clock_m = 12;
}

void CPU::LDBC_A()
{
    this->memory->writeByte((B << 8) | C, A);
    this->clock_m = 8;
}

void CPU::INC_BC()
{
    Uint8 T = (((B << 8) | C) + 1) & 0xFFFF;
    C = T & 0xFF;
    this->clock_m = 8;
}

void CPU::bindMemory(Memory* memory)
{
    this->memory = memory;
}
