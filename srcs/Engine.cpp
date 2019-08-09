#include "Engine.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>


Engine42::Engine          Engine42::Engine::_inst = Engine();
Engine42::Engine::Engine(void){
    _skybox = nullptr;
}

Engine42::Engine::~Engine(void)
{
    std::list<Shader *> shaders;
    Shader *shader = nullptr;

    if (_skybox)
        delete _skybox;
    
    std::for_each(_inst._meshRenderers.begin(), _inst._meshRenderers.end(),[&shaders] (MeshRenderer *x) -> void {
        shaders.push_back(x->GetShader());
        });
    shaders.sort();
    shaders.unique();
    while ((shader = shaders.front()) != nullptr)
    {
        shaders.pop_front();
        delete shader;
    }
}

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
void Engine42::Engine::SetSkybox(Skybox *skybox)
{
    _inst._skybox = skybox;
}

void            Engine42::Engine::AddGameObject(std::list<Engine42::IGameObject*> objs)
{
    _inst._gameObjs.insert(_inst._gameObjs.begin(), objs.begin(), objs.end());
}

const SDL_Event &Engine42::Engine::GetEvent(){ return _inst._event;}
const Uint8 *Engine42::Engine::GetKeyInput(){ return _inst._keys;}

void            Engine42::Engine::Loop(void)
{
    bool        quit = false;
    float       delta = ((float)SDL_GetTicks()) / 1000;
    float       lastTime = delta;
    const float fixedTimeUpdate = 0.02f;
    float       fixedDelta = 0.02f;
	float		LastTime = 0.0f;
	int nbFrame = 0;

    while (!quit)
    {
        delta = (((float)SDL_GetTicks()) / 1000) - lastTime;
        Time::SetDeltaTime(delta);
        _inst._event.type = SDL_USEREVENT;
        while (SDL_PollEvent(&_inst._event) != 0)
        {
			if (_inst._event.type == SDL_MOUSEMOTION)
				Camera::instance->LookAround(_inst._event.motion.xrel, -_inst._event.motion.yrel);
            if ((_inst._event.type == SDL_WINDOWEVENT 
            && _inst._event.window.event == SDL_WINDOWEVENT_CLOSE)
            || (_inst._event.type == SDL_KEYDOWN 
            && _inst._event.key.keysym.sym == SDLK_ESCAPE))
                quit = true;
        }
        _inst._keys = SDL_GetKeyboardState(NULL);
        _inst._UpdateAll();
        lastTime += delta;
		fixedDelta += delta;
		if (fixedDelta >= fixedTimeUpdate)
		{
            Time::SetFixedDeltaTime(fixedDelta);
            _inst._FixedUpdateAll();
			fixedDelta = 0.0f;
		}
		if ((((float)SDL_GetTicks()) / 1000) - LastTime >= 1.0f)
		{
			std::cout << "\r                     ";//clean line
			std::cout << "\rFPS: "<< nbFrame << std::flush;
			nbFrame = 0;
			LastTime += 1.0f;
		}
        _inst._RenderAll();
		nbFrame++;
    }
}

bool      Engine42::Engine::Destroy(MeshRenderer *meshRenderer)
{
    if (meshRenderer == nullptr)
        return false;
    Shader *shader = meshRenderer->GetShader();
    _inst._meshRenderers.remove(meshRenderer);
    if (shader != nullptr)
    {
        auto it = std::find_if(_inst._meshRenderers.begin(), _inst._meshRenderers.end(),[shader] (MeshRenderer *x) -> bool {return x->GetShader() == shader;});
        if (it == _inst._meshRenderers.end())
        {
            delete shader;
        }
    }
    delete meshRenderer;
    return true;
}
bool		_sort(const MeshRenderer* first, const MeshRenderer* sec)
{
	float d1 = glm::distance(first->transform.position, Camera::instance->GetPos());
	float d2 = glm::distance(sec->transform.position, Camera::instance->GetPos());
	return d2 < d1;
}

void                         Engine42::Engine::_RenderAll(void)
{
	_meshRenderers.sort(_sort);
    std::list<MeshRenderer*>::iterator  it;
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (_skybox != nullptr)
    {
        _skybox->Draw();
    }
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
void                       Engine42::Engine::ReloadShaders(void)
{
    std::for_each(_inst._meshRenderers.begin(), _inst._meshRenderers.end(), [] (MeshRenderer *x) -> void { 
        Shader *shader = x->GetShader(); 
        if (shader)
            shader->Reload();
        });
}
void                       Engine42::Engine::_FixedUpdateAll(void) 
{
    std::list<IGameObject*>::iterator  it;

    for (it = _gameObjs.begin(); it != _gameObjs.end(); it++)
    {
        (*it)->FixedUpdate();
    } 
}
