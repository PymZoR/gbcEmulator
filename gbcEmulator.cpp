#ifdef EMSCRIPTEN
  #include <emscripten.h>
#endif
#include <iostream>
#include <SDL/SDL.h>

using namespace std;


// Native GBC display info
const int NATIVE_WIDTH   = 160;
const int NATIVE_HEIGHT  = 144;
const float NATIVE_RATIO = 160.0 / 144;


/**
 * Resize game surface depending on the window
 * @param windowWidth  window width
 * @param WindowHeight window height
 */
void resize(int windowWidth, int WindowHeight)
{
    float screenRatio = (float)windowWidth / WindowHeight;

    float gameWidth;
    float gameHeight;

    if (screenRatio > NATIVE_RATIO) {
        gameWidth = NATIVE_WIDTH * ((float)WindowHeight / NATIVE_HEIGHT);
        gameHeight = WindowHeight;
    }
    else {
        gameWidth = windowWidth;
        gameHeight = NATIVE_HEIGHT * ((float)windowWidth / NATIVE_WIDTH);
    }

    //TODO : create/update game surface to gameWidth/gameHeight and center it
}


/**
 * Draw next frame
 */
void drawScreen()
{
    //TODO: control FPS with emscripten
    SDL_Surface* window = SDL_GetVideoSurface();
    SDL_Flip(window);

    //Fill window surface with weird green
    SDL_FillRect(window, NULL, 0xF000FF00);
}


void processEvents()
{
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                exit(EXIT_SUCCESS);
                break;

            case SDL_VIDEORESIZE:
                resize(event.resize.w, event.resize.h);
        }
    }
}


/**
 * One iteration of the main loop
 */
void oneIteration()
{
    processEvents();
    drawScreen();
}


/**
 * Entry point
 */
int main(int argc, char** argv)
{
    #ifndef EMSCRIPTEN
        atexit(SDL_Quit);
    #endif

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Video initialization failed: " << SDL_GetError() << endl;
        exit(EXIT_FAILURE);
    }

    const SDL_VideoInfo* screenInfo = SDL_GetVideoInfo();
    int screenWidth = screenInfo->current_w;
    int screenHeight = screenInfo->current_h;
    int bpp = 32;
    int flags = SDL_HWSURFACE | SDL_RESIZABLE | SDL_DOUBLEBUF;

    if (SDL_SetVideoMode(screenWidth, screenHeight, bpp, flags) == 0) {
        printf("Video mode set failed: %s\n", SDL_GetError());
        exit(EXIT_SUCCESS);
    }
    SDL_WM_SetCaption("gbcEmulator", NULL);
    resize(screenWidth, screenHeight);


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

    return EXIT_SUCCESS;
}