#include "Memory.hpp"


Memory::Memory(CPU* cpu, GPU* gpu, Key* key, Timer* timer)
{

    this->biosMapped = false;
    this->cpu = cpu;
    this->gpu = gpu;
    this->key = key;
    this->timer = timer;

    this->mbc_rombank = 0;
    this->mbc_rambank = 0;
    this->mbc_ramon = 0;
    this->mbc_mode = 0;

    this->romoffs = 0x4000;
    this->ramoffs = 0;

    this->ie = 0;
    this->if_ = 0;
    this->carttype = 0;
}

Memory::~Memory()
{

}

Uint8 Memory::rb(Uint16 addr) {
    if (addr < 0 || addr > 0xFFFF) {
        return 0;
    }

    // BIOS (256b) OR ROM0 if BIOS is mapped
    if (addr <= 0x0FFF) {
        if (this->biosMapped) {
            if (addr < 0x0100) {
                return this->bios[addr];
            }
            else if (this->cpu->PC == 0x0100) {
                this->biosMapped = true;
            }
        }

        return this->rom[addr];
    }

    // ROM0 and ROM1
    if (addr <= 0x7FFF) {
        return this->rom[addr];
    }

    // VRAM
    if (addr <= 0x9FFF) {
        return this->gpu->vram[addr & 0x1FFF];
    }

    // ERAM
    if (addr <= 0xBFFF) {
        return this->eram[addr & 0x1FFF];
    }

    // WRAM and WRAM Shadow
    if (addr <= 0xFDFF) {
        return this->wram[addr & 0x1FFF];
    }

    // Rest
    switch(addr & 0x0F00)
    {
        // Echo RAM
        case 0x000: case 0x100: case 0x200: case 0x300:
        case 0x400: case 0x500: case 0x600: case 0x700:
        case 0x800: case 0x900: case 0xA00: case 0xB00:
        case 0xC00: case 0xD00:
            return this->wram[addr & 0x1FFF];

        // OAM
        case 0xE00:
            return ((addr & 0xFF) < 0xA0) ? this->gpu->oam[addr & 0xFF] : 0;

        // ZRAM, IO, Interrupts
        case 0xF00:
            if (addr == 0xFFFF) { return this->ie; }
            if (addr > 0xFF7F) { return this->zram[addr & 0x7F]; }

            switch (addr & 0xF)
            {
                case 0x00:
                    switch(addr&0xF)
                    {
                        case 0:
                            return this->key->rb();
                        case 4: case 5: case 6: case 7:
                            this->timer->rb(addr);
                        case 15:
                            return this->if_;
                        default:
                            return 0;
                    }
                case 0x10: case 0x20: case 0x30:
                    return 0;
                case 0x40: case 0x50: case 0x60: case 0x70:
                    return this->gpu->rb(addr);
            }
    }
}

Uint16 Memory::rw(Uint16 addr) {
    return this->rb(addr) + (this->rb(addr + 1) << 8);
}

void Memory::wb(Uint16 addr, Uint8 value) {
    if (addr <= 0x1FFF) {
        if (this->carttype == 1) {
            this->mbc_ramon = ((value & 0xF) == 0xA) ? true : false;
        }
        return;
    }

    if (addr <= 0x3FFF) {
        if (this->carttype == 1) {
            this->mbc_rombank &= 0x60;
            value &= 0x1F;

            value = (value) ? value : 1;
            this->mbc_rombank |= value;
            this->romoffs = this->mbc_rombank * 0x4000;
            return;
        }
    }

    if (addr <= 0x5FFF) {
        if (this->carttype == 1) {
            if (this->mbc_mode) {
                this->mbc_rambank = value & 3;
                this->ramoffs = this->mbc_rambank * 0x2000;
            }
            else {
                this->mbc_rombank &= 0x1F;
                this->mbc_rombank |= ((value & 3) << 5);
                this->romoffs = this->mbc_rombank * 0x4000;
            }
            return;
        }
    }

    if (addr <= 0x7FFF) {
        if (this->carttype == 1) {
            this->mbc_mode = value & 1;
            return;
        }
    }

    if (addr <= 0x9FFF) {
        this->gpu->vram[addr & 0x1FFF] = value;
        this->gpu->updatetile(addr & 0x1FF, value);
        return;
    }

    if (addr <= 0xBFFF) {
        this->eram[this->ramoffs + (addr & 0x1FFF)] = value;
        return;
    }

    if (addr <= 0xEFFF) {
        this->wram[addr & 0x1FFF] = value;
        return;
    }

    switch (addr & 0x0F00) {
        // Echo RAM
        case 0x000: case 0x100: case 0x200: case 0x300:
        case 0x400: case 0x500: case 0x600: case 0x700:
        case 0x800: case 0x900: case 0xA00: case 0xB00:
        case 0xC00: case 0xD00:
            this->wram[addr & 0x1FFF] = value;
            return;
        case 0xE00:
            if ((addr & 0xFF) < 0xA0) { this->gpu->oam[addr & 0xFF] = value; }
            this->gpu->updateoam(addr, value);
            return;
        case 0xF00:
            if (addr == 0xFFFF) { this->ie = value; return; }
            if (addr > 0xFF7F) { this->zram[addr & 0x7F] = value; return; }
            switch (addr & 0xF0)
            {
                case 0x00:
                    switch (addr & 0xF)
                    {
                        case 0:
                            this->key->wb(value);
                            return;
                        case 4: case 5: case 6: case 7:
                            this->timer->wb(addr, value);
                            return;
                        case 15:
                            this->if_ = value;
                            return;
                    }
                case 0x40: case 0x50: case 0x60: case 0x70:
                    this->gpu->wb(addr, value);
                    return;

            }
    }
}

void Memory::ww(Uint16 addr, Uint16 value) {
    this->wb(addr, value & 255);
    this->wb(addr + 1, value >> 8);
}

void Memory::load(Uint8* file, size_t len) {
    memmove(this->rom, file, len * sizeof(Uint8));
}
