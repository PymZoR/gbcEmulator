#ifdef EMSCRIPTEN
  #include <emscripten.h>
#endif
#include <iostream>
#include <SDL/SDL.h>

using namespace std;


// Native GBC display info
const int NATIVE_WIDTH   = 160;
const int NATIVE_HEIGHT  = 144;


int game_w;
int game_h;
int pixel_size_w = 5;
int pixel_size_h = 5;
bool surface_drawn = false;
bool keep_ratio = false;


// SDL Variables
int bpp = 32;
int flags = SDL_HWSURFACE | SDL_RESIZABLE | SDL_DOUBLEBUF;
SDL_Surface* screen;
SDL_Surface* window;


/**
  * Update game dimensions
  */
void resize(int w, int h) {
    cout << SDL_GetVideoSurface() << endl;
    cout << window << endl;
    game_w = w;
    game_h = h;
}


/**
 * Draw a big pixel
 */
void drawPixel(int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    int containerWidth = game_w;
    int containedWidth = NATIVE_WIDTH * pixel_size_w;
    int containerHeight = game_h;
    int containedHeight = NATIVE_HEIGHT * pixel_size_h;

    int offsetX = (int)(((float)containerWidth / 2) - ((float)containedWidth / 2));
    int offsetY = (int)(((float)containerHeight / 2) - ((float)containedHeight / 2));

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
        SDL_FillRect(window, NULL, SDL_MapRGB(window->format, 255, 255, 255));
        //surface_drawn = true;

        int pixelSizeW = game_w / NATIVE_WIDTH;
        int pixelSizeH = game_h / NATIVE_HEIGHT;

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

        SDL_Delay(100);
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
                screen = SDL_SetVideoMode(event.resize.w, event.resize.h, bpp,
                                          flags);
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
    int screenWidth = screenInfo->current_w / 2;
    int screenHeight = screenInfo->current_h / 2;

    screen = SDL_SetVideoMode(screenWidth, screenHeight, bpp, flags);
    if (screen == NULL) {
        cout << "Video mode set failed: " << SDL_GetError() << endl;
        exit(EXIT_SUCCESS);
    }
    SDL_WM_SetCaption("gbcEmulator", NULL);

    window = SDL_GetVideoSurface();
    resize(window->w, window->h);

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
