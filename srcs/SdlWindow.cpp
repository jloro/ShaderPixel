#include "SdlWindow.hpp"
SdlWindow      *SdlWindow::_mainWin = nullptr;

SdlWindow::SdlWindow(int width, int height, bool isRenderer, bool isOpenGl, std::string name)
{
    if (width < 1 || height < 1)
        throw std::invalid_argument("width and height must be > 0");
    if (name.empty())
        throw std::invalid_argument("window name cannot be empry");
    int flags;
    if (isOpenGl)
        flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    else
        flags = SDL_WINDOW_SHOWN;
    _width = width;
    _height = height;
    _renPtr = nullptr;
    _glContext = 0;
    if (!(_winPtr = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags)))
        throw std::runtime_error("Fail to create window");
    if (isRenderer)
    {
        _renPtr = SDL_CreateRenderer(_winPtr, -1, 0);
    }
    if (SdlWindow::_mainWin != nullptr)
        SdlWindow::_mainWin = this;
}

void    SdlWindow::CreateGlContext(int majVersion, int minVersion, bool doublebuffer, int depthSize)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, majVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minVersion);
    if (doublebuffer)
    {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    }
    else
    {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);
    }
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, depthSize);
    if (!(_glContext = SDL_GL_CreateContext(_winPtr)))
    {
        throw std::runtime_error("fail to create OpenGl context");
    }
    SDL_GL_MakeCurrent(_winPtr, _glContext);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        throw std::runtime_error("fail to load Glad functions");
    }
}
void       SdlWindow::Swap(void) const
{
    SDL_GL_SwapWindow(_winPtr);
}
SdlWindow::~SdlWindow(void)
{
    if (this->_renPtr)
    {
        SDL_DestroyRenderer(_renPtr);
        this->_renPtr = nullptr;
    }
    if (this->_winPtr)
    {
        SDL_DestroyWindow(this->_winPtr);
        this->_winPtr = nullptr;
    }
}
#pragma region Getters_Setters
int     SdlWindow::GetWidth(void)const { return _width;}
int     SdlWindow::GetHeight(void) const {return _height;}
SDL_Window const *SdlWindow::GetWinPtr(void) const{return _winPtr;}
SDL_Renderer const *SdlWindow::GetRendererPtr(void) const{return _renPtr;}
void    SdlWindow::SetSize(int width, int height)
{
    if (width < 1 || height < 1)
        throw std::invalid_argument("width and height must be > 0");
    _width = width;
    _height = height;
    SDL_SetWindowSize(_winPtr, _width, _height);
}
void    SdlWindow::SetResizable(bool state)
{
    if (state)        
        SDL_SetWindowResizable(_winPtr, SDL_TRUE);
    else
        SDL_SetWindowResizable(_winPtr, SDL_FALSE);
}
#pragma endregion Getters_Setters
std::string const SdlWindow::toString(void) const
{
    return  std::string(SDL_GetWindowTitle(_winPtr));
}
std::ostream &	operator<< (std::ostream & o, SdlWindow const & rhs)
{
    o << rhs.toString();
    return o;
}