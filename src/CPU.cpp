#include "CPU.hpp"


// Flag shortcuts
#define z 0b10000000
#define n 0b01000000
#define h 0b00100000
#define c 0b00010000


CPU::CPU(Memory* memory):
AF(_AF.word), BC(_BC.word), DE(_DE.word), HL(_HL.word),
A(_AF.hb), F(_AF.lb), B(_BC.hb), C(_BC.lb), D(_DE.hb),
E(_DE.lb), H(_HL.hb), L(_HL.lb)
{
    this->memory = memory;
    this->opcodesList[0x00] = &CPU::opFuncTest;

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


void CPU::opFuncTest()
{
    cout << "opFuncTest" << endl;
}