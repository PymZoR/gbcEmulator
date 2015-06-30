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


int screenWidth;
int screenHeight;
int pixel_size_w = 5;
int pixel_size_h = 5;
bool surface_drawn = false;
bool keep_ratio = false;


// SDL Variables
SDL_Surface* window;


/**
 * Resize game surface depending on the window
 * @param windowWidth  window width
 * @param windowHeight window height
 */
void resize(int windowWidth, int windowHeight)
{
    float screenRatio = (float)windowWidth / windowHeight;

    float gameWidth;
    float gameHeight;

    if (screenRatio > NATIVE_RATIO) {
        gameWidth = NATIVE_WIDTH * ((float)windowHeight / NATIVE_HEIGHT);
        gameHeight = windowHeight;
    }
    else {
        gameWidth = windowWidth;
        gameHeight = NATIVE_HEIGHT * ((float)windowWidth / NATIVE_WIDTH);
    }

    //TODO : create/update game surface to gameWidth/gameHeight and center it
}


/**
 * Draw a big pixel
 */
void drawPixel(int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    int containerWidth = window->w;
    int containedWidth = NATIVE_WIDTH * pixel_size_w;
    int containerHeight = window->h;
    int containedHeight = NATIVE_HEIGHT * pixel_size_h;

    int offsetX = (int)(((float)containerWidth / 2) - ((float)containedWidth / 2));
    int offsetY = (int)(((float)containerHeight / 2) - ((float)containedHeight / 2));

    cout << offsetY << endl;

    x = x * pixel_size_w + offsetX;
    y = y * pixel_size_h + offsetY;

    SDL_Rect rect = { x, y, pixel_size_w, pixel_size_h };
    SDL_FillRect(window, &rect, SDL_MapRGB(window->format, r, g, b));
}

/**
 * Draw next frame
 */
void drawScreen()
{
    //TODO: control FPS with emscripten
    SDL_Flip(window);

    if (!surface_drawn) {
        surface_drawn = true;

        int pixelSizeW = window->w / NATIVE_WIDTH;
        int pixelSizeH = window->h / NATIVE_HEIGHT;

        if (!keep_ratio) {
            pixel_size_w = pixelSizeW;
            pixel_size_h = pixelSizeH;
        }
        else {
            pixel_size_w = min(pixelSizeW, pixelSizeH);
            pixel_size_h = pixel_size_w;
        }

        for (size_t i = 0; i < NATIVE_WIDTH; i++) {
            for (size_t j = 0; j < NATIVE_HEIGHT; j++) {
                drawPixel(i, j, rand() % 256, rand() % 256, rand() % 256);
            }
        }
    }
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
    screenWidth = screenInfo->current_w / 2;
    screenHeight = screenInfo->current_h / 2;
    int bpp = 32;
    int flags = SDL_HWSURFACE | SDL_RESIZABLE | SDL_DOUBLEBUF;

    if (SDL_SetVideoMode(screenWidth, screenHeight, bpp, flags) == 0) {
        cout << "Video mode set failed: " << SDL_GetError() << endl;
        exit(EXIT_SUCCESS);
    }
    SDL_WM_SetCaption("gbcEmulator", NULL);
    resize(screenWidth, screenHeight);

    window = SDL_GetVideoSurface();

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
