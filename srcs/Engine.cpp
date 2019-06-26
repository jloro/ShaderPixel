#include "Engine.hpp"
#include <iostream>
Engine42::Engine          Engine42::Engine::_inst = Engine();
Engine42::Engine::Engine(void){}

Engine42::Engine::~Engine(void){}

void            Engine42::Engine::SetWindow(const SdlWindow *win) {_inst._win = win;}
void            Engine42::Engine::AddMeshRenderer(std::list<MeshRenderer*> meshRenderers)
{
    _inst._meshRenderers.insert(_inst._meshRenderers.end(), meshRenderers.begin(), meshRenderers.end());
}
void            Engine42::Engine::AddMeshRenderer(MeshRenderer *meshRenderers) 
{
    if (meshRenderers != nullptr)
        _inst._meshRenderers.push_back(meshRenderers);
}
void            Engine42::Engine::AddGameObject(Engine42::IGameObject *object)
{
    if (object != nullptr)
        _inst._gameObjs.push_back(object);
}
void            Engine42::Engine::AddGameObject(std::list<Engine42::IGameObject*> objs)
{
    _inst._gameObjs.insert(_inst._gameObjs.begin(), objs.begin(), objs.end());
}
const SDL_Event &Engine42::Engine::GetInput(){ return _inst._event;}

void            Engine42::Engine::Loop(void)
{
    bool        quit = false;
    float       delta = ((float)SDL_GetTicks()) / 1000;
    float       lastTime = delta;
    const float fixedTimeUpdate = 0.02f;
    float       fixedDelta = 0.02f;

    //lastTime = SDL_GetTicks();

    while (!quit)
    {
        delta = (((float)SDL_GetTicks()) / 1000) - lastTime;
        std::cout << "Ticks = " << (float)SDL_GetTicks() <<"  delta =" 
        << delta << "  lastime = " << lastTime << std::endl;
        Time::SetDeltaTime(delta);
        while (SDL_PollEvent(&_inst._event) != 0)
        {
            if ((_inst._event.type == SDL_WINDOWEVENT 
            && _inst._event.window.event == SDL_WINDOWEVENT_CLOSE)
            || (_inst._event.type == SDL_KEYDOWN 
            && _inst._event.key.keysym.sym == SDLK_ESCAPE))
                quit = true;
            _inst._UpdateAll();
        }
		//state = SDL_GetKeyboardState(NULL);
        lastTime += delta;
		fixedDelta += delta;
		if (fixedDelta >= fixedTimeUpdate)
		{
            Time::SetFixedDeltaTime(fixedDelta);
            _inst._FixedUpdateAll();
			fixedDelta = 0.0f;
		}
        _inst._RenderAll();
    }
}

void                         Engine42::Engine::_RenderAll(void)
{
    std::list<MeshRenderer*>::iterator  it;
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (it = _meshRenderers.begin(); it != _meshRenderers.end(); it++)
    {
         (*it)->Draw();
    }
	_win->Swap();
}
void                          Engine42::Engine::_UpdateAll(void)
{
    std::list<IGameObject*>::iterator  it;

    for (it = _gameObjs.begin(); it != _gameObjs.end(); it++)
    {
        (*it)->Update();
    }
}
void                           Engine42::Engine::_FixedUpdateAll(void) 
{
    std::list<IGameObject*>::iterator  it;

    for (it = _gameObjs.begin(); it != _gameObjs.end(); it++)
    {
        (*it)->FixedUpdate();
    } 
}