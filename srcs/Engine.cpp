#include "Engine.hpp"
#include <iostream>

Engine42::Engine::Engine(void)
{
    
}

Engine42::Engine::~Engine(void)
{
    
}
void            Engine42::Engine::SetWindow(const SdlWindow *win) {_win = win;}
void            Engine42::Engine::AddModel(std::list<Model*> models)
{
    _models.insert(_models.end(), models.begin(), models.end());
}
void            Engine42::Engine::AddModel(Model *model) 
{
    if (model != nullptr)
        _models.push_back(model);
}
void            Engine42::Engine::AddGameObject(Engine42::IGameObject *object)
{
    if (object != nullptr)
        _gameObjs.push_back(object);
}
void            Engine42::Engine::AddGameObject(std::list<Engine42::IGameObject*> objs)
{
    _gameObjs.insert(_gameObjs.begin(), objs.begin(), objs.end());
}
void            Engine42::Engine::Loop(void)
{
    bool quit = false;
    float delta = SDL_GetTicks();
    float lastTime = SDL_GetTicks();
    float fixedDelta = 0.02f;

    while (!quit)
    {
        while (SDL_PollEvent(&_event) != 0)
        {
            if (_event.type == SDL_WINDOWEVENT && _event.window.event
				== SDL_WINDOWEVENT_CLOSE)
                quit = true;
            _UpdateAll();
        }
		//state = SDL_GetKeyboardState(NULL);

		delta += SDL_GetTicks() - lastTime;
		/*if (delta >= fixedDelta)
		{
			processInput(state, quit, 0.01f);
			delta = 0.0f;
		}*/
		lastTime = SDL_GetTicks();
        _RenderAll();
    }
}
const SDL_Event &Engine42::Engine::GetInput(){ return _event;}

void                         Engine42::Engine::_RenderAll(void)
{
    std::list<Model*>::iterator  it;
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// myShader.use();
		// myShader.setMat4("view", cam.GetMatView());
		// myShader.setMat4("projection", cam.GetMatProj());
    for (it = _models.begin(); it != _models.end(); it++)
    {
        // (*it)->Draw(myShader);
         (*it)->Draw();
    }


		/*for (auto pos : positions)
		{
			pillar.SetModel(glm::translate(glm::mat4(1.0f), pos));
			pillar.Draw(myShader);
		}*/
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