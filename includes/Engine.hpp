#ifndef ENGINE_HPP
# define ENGINE_HPP

# include <iostream>
# include <list>
# include "Model.hpp"
# include "IGameObject.hpp"

namespace Engine42
{
    class Engine 
    {

    public: 
    /*	constructors / destrucors	*/
        virtual ~Engine(void); 

    /*	public variables	*/
    /*	public functions	*/
        static Engine   &inst;
        void            AddModel(std::list<Model*> models);
        void            AddModel(Model *model);
        void            AddGameObject(Engine42::IGameObject *object);
        void            AddGameObject(std::list<Engine42::IGameObject> *objects);
        void            Loop(void);
        SDL_Event       *input;
    private:
    /*  private constructor*/
        Engine(void); 
    /*	private variables	*/
        std::list<Model*>                    _models;
        std::list<Engine42::IGameObject*>   _gameObjs;
    /*	private functions	*/
        void                                _RenderAll(void);
        void                                _UpdateAll(void);
        void                                _FixedUpdateAll(void);
    };
}

#endif


#include "Engine.hpp"
#include <iostream>

Engine::Engine(void)
{
    
}

Engine::~Engine(void)
{
    
}