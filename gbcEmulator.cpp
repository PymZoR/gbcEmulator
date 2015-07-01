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
bool keep_ratio = true;


// Map
Uint32 pixels_map[NATIVE_WIDTH][NATIVE_HEIGHT] = { { 0 } };
Uint32 pixels_map_actual[NATIVE_WIDTH][NATIVE_HEIGHT] = { { 1 } };


// SDL Variables
int bpp = 32;
int flags = SDL_HWSURFACE | SDL_RESIZABLE | SDL_DOUBLEBUF;
SDL_Surface* screen;
SDL_Surface* window;


/**
 * Update game dimensions
 */
void resize(int w, int h) {
    game_w = w;
    game_h = h;

    int new_pixel_size_w = game_w / NATIVE_WIDTH;
    int new_pixel_size_h = game_h / NATIVE_HEIGHT;

    if (!keep_ratio) {
        pixel_size_w = new_pixel_size_w;
        pixel_size_h = new_pixel_size_h;
    }
    else {
        pixel_size_w = min(new_pixel_size_w, new_pixel_size_h);
        pixel_size_h = pixel_size_w;
    }
}


/**
 * Draw a big pixel
 */
void drawPixel(int x, int y, Uint32 color)
{
    int container_width = game_w;
    int contained_width = NATIVE_WIDTH * pixel_size_w;
    int container_height = game_h;
    int contained_height = NATIVE_HEIGHT * pixel_size_h;

    int offset_x = (int)(((float)container_width / 2) - ((float)contained_width / 2));
    int offset_y = (int)(((float)container_height / 2) - ((float)contained_height / 2));

    x = x * pixel_size_w + offset_x;
    y = y * pixel_size_h + offset_y;

    SDL_Rect rect = { x, y, pixel_size_w, pixel_size_h };
    SDL_FillRect(window, &rect, color);
}

/**
 * Draw next frame
 */
void drawScreen(bool force = false)
{
    //TODO: control FPS with emscripten
    SDL_Flip(window);

    for (size_t i = 0; i < NATIVE_WIDTH; i++) {
        for (size_t j = 0; j < NATIVE_HEIGHT; j++) {
            if (pixels_map[i][j] != pixels_map_actual[i][j] || force) {
                pixels_map_actual[i][j] = pixels_map[i][j];
                drawPixel(i, j, pixels_map[i][j]);
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
                SDL_FreeSurface(screen);
                screen = SDL_SetVideoMode(event.resize.w, event.resize.h, bpp,
                                          flags);
                resize(event.resize.w, event.resize.h);
                drawScreen(true);
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

    const SDL_VideoInfo* screen_info = SDL_GetVideoInfo();
    int screen_width  = screen_info->current_w / 2;
    int screen_height = screen_info->current_h / 2;

    #ifdef EMSCRIPTEN
        screen_width  = screen_info->current_w;
        screen_height = screen_info->current_h;
    #endif

    screen = SDL_SetVideoMode(screen_width, screen_height, bpp, flags);
    if (screen == NULL) {
        cout << "Video mode set failed: " << SDL_GetError() << endl;
        exit(EXIT_SUCCESS);
    }
    SDL_WM_SetCaption("gbcEmulator", NULL);

    window = SDL_GetVideoSurface();
    resize(window->w, window->h);

    // Generate pixels
    srand (time(NULL));
    for (size_t i = 0; i < NATIVE_WIDTH; i++) {
        for (size_t j = 0; j < NATIVE_HEIGHT; j++) {
            pixels_map[i][j] = SDL_MapRGB(window->format, rand() % 256, rand() % 256, rand() % 256);
        }
    }

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
