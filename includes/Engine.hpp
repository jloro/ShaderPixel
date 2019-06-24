#ifndef ENGINE_HPP
# define ENGINE_HPP

# include <iostream>
# include <list>
# include "Model.hpp"
# include "IGameObject.hpp"
# include "SdlWindow.hpp"

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
        void            SetWindow(const SdlWindow *win);
        void            AddGameObject(Engine42::IGameObject *object);
        void            AddGameObject(std::list<Engine42::IGameObject*> objects);
        void            Loop(void);
        const SDL_Event &GetInput();
    private:
    /*  private constructor*/
        Engine(void); 
    /*	private variables	*/
        std::list<Model*>                    _models;
        std::list<Engine42::IGameObject*>   _gameObjs;
        SDL_Event                           _event;
        const SdlWindow                     *_win;           
    /*	private functions	*/
        void                                _RenderAll(void);
        void                                _UpdateAll(void);
        void                                _FixedUpdateAll(void);
    };
}

#endif