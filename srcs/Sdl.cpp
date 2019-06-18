#include "Sdl.hpp"
#include <iostream>

Sdl::Sdl(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::runtime_error(SDL_GEtError());
    }
}

Sdl::Sdl(Sdl const & src) 
{
    *this = src;
    return;
}

Sdl::~Sdl(void)
{
    SDL_Quit();
}

Sdl &	Sdl::operator=(Sdl const & rhs)
{
    return *this;
}

std::string const Sdl::toString(void) const
{
    
}
std::ostream &	operator<< (std::ostream & o, Sdl const & rhs)
{
    o << rhs.toString();
    return o;
}