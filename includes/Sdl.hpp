#ifndef Sdl_HPP
# define Sdl_HPP

# include <iostream>
# include "SDL.h"
# include "SdlWindow.hpp"

class Sdl 
{
public: 
    Sdl(int glMaj, int glMin); 
    Sdl(Sdl const & src); 
    ~Sdl(void); 

	Sdl &		operator=(Sdl const & rhs);

    std::string const	toString(void) const;
    static Sdl  &instance;

private:
    SdlWindow   win;
    Sdl(void); 
};

std::ostream &	operator<< (std::ostream & o, Sdl const & rhs);

#endif