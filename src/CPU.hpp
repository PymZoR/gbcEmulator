    #ifndef CPU_HPP
#define CPU_HPP

#include <iostream>
#include <iomanip>
#include <map>
#include <SDL/SDL.h>


using namespace std;


/*
    Forward declarations
 */
class Memory;
class CPU;


/*
    Types
 */
typedef void(CPU::*opFunction)();


union Reg
{
    Uint16 word;
    struct {
        Uint8 lb;
        Uint8 hb;
    };
};


class CPU
{
    public:
        CPU();
        ~CPU();
        void reset();
        void oneIteration();
        void bindMemory(Memory* memory);

        Uint16 clock_m;
        Uint16 clock_t;

        // 16-bit registers
        Uint16 SP;
        Uint16 PC;
        Uint16& AF;
        Uint16& BC;
        Uint16& DE;
        Uint16& HL;

        // 8-bit registers
        Uint8& A;
        Uint8& F;
        Uint8& B;
        Uint8& C;
        Uint8& D;
        Uint8& E;
        Uint8& H;
        Uint8& L;


    private:
        void noop();
        void LDBC_nn();
        void LDBC_A();
        void INC_BC();

        Reg _AF;
        Reg _BC;
        Reg _DE;
        Reg _HL;

        opFunction opcodesList[2];
        Memory* memory;
};

#endif
