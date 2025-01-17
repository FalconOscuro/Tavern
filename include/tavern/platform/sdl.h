#ifndef PLATFORM_SDL_H
#define PLATFORM_SDL_H

#ifdef __linux
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL.h>
#endif

#endif /* end of define guard PLATFORM_SDL_H */
