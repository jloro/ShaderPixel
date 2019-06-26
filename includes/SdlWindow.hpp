#ifndef SDLWINDOW_HPP
# define SDLWINDOW_HPP

# include <iostream>
# include "SDL.h"
# include "glad.h"

class SdlWindow 
{
public: 
    SdlWindow(int width, int height, bool isRenderer, bool isOpenGl, std::string name);
    ~SdlWindow(void);
    void    SetSize(int width, int height);
    void    SetResizable(bool state);
    int     GetWidth(void) const;
    int     GetHeight(void) const;
    void    CreateGlContext(int majVersion, int minVersion, bool doublebuffer, int depthSize);
    void    Swap(void) const;
    SDL_Window const    *GetWinPtr(void) const;
    SDL_Renderer const  *GetRendererPtr(void) const;
    std::string const   toString(void) const;
    static SdlWindow    *GetMain(void);
private:
    int                 _width;
    int                 _height;
    SDL_Window          *_winPtr;
    SDL_Renderer        *_renPtr;
    SDL_GLContext       _glContext;
    static SdlWindow    *_mainWin;
};

#endif