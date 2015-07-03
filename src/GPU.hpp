#ifndef GPU_HPP
#define GPU_HPP

#include <SDL/SDL.h>


class GPU
{
    public:
        GPU();
        ~GPU();

        // TODO: correct length
        Uint8 readByte(Uint16 addr);
        void writeByte(Uint16 addr, Uint16 value);
        void updateTile(Uint16 addr, Uint32 value);
        void updateOam(Uint16 addr, Uint32 value);
        Uint8 vram[1];
        Uint8 oam[1];
};

#endif
