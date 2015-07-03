#ifndef TIMER_HPP
#define TIMER_HPP

#include <SDL/SDL.h>


class Timer
{
    public:
        Timer();
        ~Timer();
        Uint8 readByte(Uint16 addr);
        void writeByte(Uint16 addr, Uint16 value);
};

#endif
