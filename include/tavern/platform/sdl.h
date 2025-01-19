#ifndef PLATFORM_SDL_H
#define PLATFORM_SDL_H

#ifdef __linux
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL.h>
#endif

// Ensures use of correct funciton for getting window size
// High dpi windows can behave differently from base getwindowsize, dependent on renderer
#ifdef USE_OPENGL
#define SDL_GetDrawableSize SDL_GL_GetDrawableSize
#define SDL_WINDOW_RENDERER SDL_WINDOW_OPENGL
#else
#define SDL_GetDrawableSize SDL_GetWindowSize
#endif

#endif /* end of define guard PLATFORM_SDL_H */
