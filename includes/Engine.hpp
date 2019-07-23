#ifndef ENGINE_HPP
# define ENGINE_HPP

# include <iostream>
# include <list>
# include "MeshRenderer.hpp"
# include "IGameObject.hpp"
# include "SdlWindow.hpp"
# include "Time.hpp"

namespace Engine42
{
    class Engine 
    {

    public: 
    /*	constructors / destrucors	*/
        virtual ~Engine(void); 

    /*	public variables	*/
    /*	public functions	*/
        static void            AddMeshRenderer(std::list<MeshRenderer*> meshrenderer);
        static void            AddMeshRenderer(MeshRenderer *meshrenderer);
        static void            SetWindow(const SdlWindow *win);
        static void            AddGameObject(Engine42::IGameObject *object);
        static void            AddGameObject(std::list<Engine42::IGameObject*> objects);
        static void            Loop(void);
        static const SDL_Event &GetEvent();
        static const Uint8     *GetKeyInput();
    private:
    /*  private constructor*/
        Engine(void); 
    /*	private variables	*/
        static Engine                       _inst;
        std::list<MeshRenderer*>            _meshRenderers;
        std::list<Engine42::IGameObject*>   _gameObjs;
        SDL_Event                           _event;
        const Uint8                         *_keys;
        const SdlWindow                     *_win;           
    /*	private functions	*/
        void                                _RenderAll(void);
        void                                _UpdateAll(void);
        void                                _FixedUpdateAll(void);
    };
}

#endif
