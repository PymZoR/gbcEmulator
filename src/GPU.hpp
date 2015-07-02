#ifndef GPU_HPP
#define GPU_HPP

#ifdef EMSCRIPTEN
  #include <emscripten.h>
#endif
#include <iostream>
#include <SDL/SDL.h>

using namespace std;


class GPU
{
    public:
        GPU();
        ~GPU();

        /**
         * Draw a pixel scaled on the game surface size
         * @param x     x coordinate
         * @param y     y coordinate
         * @param color color
         */
        void drawPixel(Uint16 x, Uint16 y, Uint32 color);

        /**
         * One clock-cycle of GPU
         */
        void oneIteration();

        // GBC native resolution
        static const int NATIVE_WIDTH = 160;
        static const int NATIVE_HEIGHT = 144;


    private:
        /**
         * Window initialization
         */
        void init();

        /**
         * Resize game surface size depending on window size
         * @param w window new with
         * @param h window new height
         */
        void resize(int w, int h);

        /**
         * Draw the entire game screen
         */
        void drawScreen();

        /**
         * Handle SDL events
         */
        void processEvents();

        /**
         * Game surface pixel map
         */
        Uint32 pixelMap[GPU::NATIVE_WIDTH][GPU::NATIVE_HEIGHT];

        int windowWidth;
        int windowHeight;

        int gameWidth;
        int gameHeight;

        int pixelWidth;
        int pixelHeight;
        bool keepRatio;

        int videoBpp;
        int videoFlags;
        SDL_Surface* window;
};

#endif