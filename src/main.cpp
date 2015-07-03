#include <iostream>
#include <SDL/SDL.h>

#include "Screen.hpp"
#include "Memory.hpp"
#include "CPU.hpp"
#include "GPU.hpp"


using namespace std;


Screen* screen;
GPU* gpu;
CPU* cpu;
Memory* memory;


void oneIteration()
{
    screen->oneIteration();
}


int main(int argc, char** argv)
{
    screen = new Screen();
    cpu = new CPU();
    gpu = new GPU();
    memory = new Memory(gpu, NULL, NULL);

    cpu->bindMemory(memory);
    memory->bindCPU(cpu);


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
