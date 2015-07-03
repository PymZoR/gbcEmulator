#include "Screen.hpp"


Screen::Screen()
{
    this->windowWidth = 0;
    this->windowHeight = 0;

    this->gameWidth = 0;
    this->gameHeight = 0;

    this->pixelWidth = 0;
    this->pixelHeight = 0;
    this->keepRatio = true;

    this->videoBpp = 32;
    this->videoFlags = SDL_HWSURFACE | SDL_RESIZABLE | SDL_DOUBLEBUF;

    this->init();
}


Screen::~Screen() { }


void Screen::init()
{
    #ifndef EMSCRIPTEN
        atexit(SDL_Quit);
    #endif

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "Video initialization failed: " << SDL_GetError() << endl;
        exit(EXIT_FAILURE);
    }

    /**
     * Screen infos
     */
    const SDL_VideoInfo* screenInfo = SDL_GetVideoInfo();
    int screenWidth  = screenInfo->current_w / 2;
    int screenHeight = screenInfo->current_h / 2;
    // TODO: reliable screen info when native
    #ifdef EMSCRIPTEN
        screenWidth  = screenInfo->current_w;
        screenHeight = screenInfo->current_h;
    #endif

    /**
     * Init window
     */
    this->window = SDL_SetVideoMode(screenWidth, screenHeight,
                                    this->videoBpp, this->videoFlags);
    if (this->window == NULL) {
        cout << "Video mode set failed: " << SDL_GetError() << endl;
        exit(EXIT_SUCCESS);
    }
    SDL_WM_SetCaption("gbcEmulator", NULL);

    resize(screenWidth, screenHeight);

    /**
     * Random pixel generation
     */
    srand(time(NULL));
    for (Uint8 i = 0; i < NATIVE_WIDTH; i++) {
        for (Uint8 j = 0; j < NATIVE_HEIGHT; j++) {
            Uint32 color = SDL_MapRGB(this->window->format, rand() % 256, rand() % 256, rand() % 256);
            this->drawPixel(i, j, color);
        }
    }
}


void Screen::drawPixel(Uint16 x, Uint16 y, Uint32 color)
{
    this->pixelMap[x][y] = color;

    Uint16 xOffset = ((float)(this->windowWidth - this->gameWidth))/2;
    Uint16 yOffset = ((float)(this->windowHeight - this->gameHeight))/2;

    x = x * this->pixelWidth + xOffset;
    y = y * this->pixelHeight + yOffset;

    SDL_Rect rect = { x, y, this->pixelWidth, this->pixelHeight };
    SDL_FillRect(window, &rect, color);
}


void Screen::resize(int w, int h) {
    windowWidth = w;
    windowHeight = h;

    int newPixelWidth = windowWidth / Screen::NATIVE_WIDTH;
    int newPixelHeight = windowHeight / Screen::NATIVE_HEIGHT;

    if (!this->keepRatio) {
        this->pixelWidth = newPixelWidth;
        this->pixelHeight = newPixelHeight;
    }
    else {
        this->pixelWidth = min(newPixelWidth, newPixelHeight);
        this->pixelHeight = pixelWidth;
    }

    this->gameWidth = this->pixelWidth * Screen::NATIVE_WIDTH;
    this->gameHeight = this->pixelHeight * Screen::NATIVE_HEIGHT;
}


void Screen::drawScreen()
{
    for (Uint8 i = 0; i < NATIVE_WIDTH; i++) {
        for (Uint8 j = 0; j < NATIVE_HEIGHT; j++) {
            drawPixel(i, j, pixelMap[i][j]);
        }
    }
}


void Screen::processEvents()
{
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                exit(EXIT_SUCCESS);
                break;

            case SDL_VIDEORESIZE:
                SDL_FreeSurface(this->window);
                this->window = SDL_SetVideoMode(event.resize.w, event.resize.h,
                                                this->videoBpp, this->videoFlags);
                this->resize(event.resize.w, event.resize.h);
                this->drawScreen();
        }
    }
}


void Screen::oneIteration()
{
    processEvents();
    SDL_Flip(window);
}
