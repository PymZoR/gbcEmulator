#ifndef MEMORY_HPP
#define MEMORY_HPP

#ifdef EMSCRIPTEN
  #include <emscripten.h>
#endif
#include <iostream>
#include <SDL/SDL.h>

#include "GPU.hpp"
#include "Key.hpp"
#include "Timer.hpp"

using namespace std;


/*
    Forward declarations
 */
class CPU;


class Memory
{
    public:
        Memory(GPU* gpu, Key* key, Timer* timer);
        ~Memory();

        /**
         * Read a byte given an address. Automatically choose the source ram
         * @param  addr  The address index
         * @return value The value at the given address
         */
        Uint8 readByte(Uint16 addr);

        /**
         * Read a word (two bytes) given an address. Automatically choose the dest ram
         * @param  addr  The address index
         * @return value The value at the given address
         */
        Uint16 readWord(Uint16 addr);

        /**
         * Writes a byte on the address. Automatically choose the source ram
         * @param addr  The address index
         * @param value The byte value
         */
         void writeByte(Uint16 addr, Uint8 value);

         /**
          * Writes a word (two bytes) given an address. Automatically choose the dest ram
          * @param  addr  The address index
          * @return value The value at the given address
          */
         void writeWord(Uint16 addr, Uint16 value);

        /**
         * Loads a game into the Memory
         * @param file The file represented as a Uint8 array
         * @param len  The length of the file
         */
         void load(Uint8* file, size_t len);

         void bindCPU(CPU* cpu);

    private:
        bool biosMapped;
        CPU* cpu;
        GPU* gpu;
        Key* key;
        Timer* timer;

        Uint8 bios[256];
        Uint8 rom[32 * 1024];
        Uint8 eram[8 * 1024];
        Uint8 wram[16 * 1024];
        Uint8 zram[128];

        Uint8 ie;
        Uint8 if_;
        Uint8 carttype;
        Uint16 ramoffs;
        Uint16 romoffs;
        Uint8 mbc_rambank;
        bool mbc_mode;
        bool mbc_rombank;
        bool mbc_ramon;
};

#endif
