#include <iostream>
#include <SDL/SDL.h>

#include "GPU.hpp"
#include "CPU.hpp"
#include "Memory.hpp"


using namespace std;


GPU* gpu;
CPU* cpu;
Memory* memory;


void oneIteration()
{
    gpu->oneIteration();
}


int main(int argc, char** argv)
{
    gpu = new GPU();
    cpu = new CPU(memory);
    /**
     * Main loop
     */
    #ifdef EMSCRIPTEN
        emscripten_set_main_loop(oneIteration, 0, 1);
    #else
        while(1) {
            oneIteration();
            //60 FPS
            SDL_Delay(16);
        }

        SDL_Quit();
    #endif
}