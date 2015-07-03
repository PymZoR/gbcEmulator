#ifndef KEY_HPP
#define KEY_HPP

#include <SDL/SDL.h>


class Key
{
    public:
        Key();
        ~Key();
        //Uint8 readByte(Uint16 addr); TODO: correct
        Uint8 readByte();
        //void writeByte(Uint16 addr, Uint16 value); TODO: correct
        void writeByte(Uint16 addr);
};

#endif
