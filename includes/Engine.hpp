#ifndef ENGINE_HPP
# define ENGINE_HPP

# include <iostream>
# include <list>

# include "IGameObject.hpp"
# include "SdlWindow.hpp"
# include "Time.hpp"
# include "Skybox.hpp"
# include "MeshRenderer.hpp"
# include "Framebuffer.hpp"
# include "PostProcess.hpp"

namespace Engine42
{
	class Engine 
	{

		public: 
			/*	constructors / destrucors	*/
			virtual ~Engine(void); 

			/*	public variables	*/
			/*	public functions	*/
			static void            AddMeshRenderer(std::list<std::shared_ptr<MeshRenderer>> meshrenderer);
			static void            AddMeshRenderer(std::shared_ptr<MeshRenderer> meshrenderer);
			static void            AddFramebuffer(std::shared_ptr<Framebuffer>  fbo);
			static void            SetWindow(const SdlWindow *win);
			static void            AddGameObject(std::shared_ptr<Engine42::IGameObject> object);
			static void            AddGameObject(std::list<std::shared_ptr<Engine42::IGameObject>> objects);
			static void            SetSkybox(std::shared_ptr<Skybox> skybox);
			static void            Loop(void);
			static const SDL_Event &GetEvent();
			static const Uint8     *GetKeyInput();
			static bool             Destroy(std::shared_ptr<MeshRenderer> meshRenderer);
			static void             ReloadShaders(void);

			static void	createFBO();
		private:
			unsigned int _fbo;
			unsigned int _colorBuffer;
			unsigned int _rbo;
			unsigned int quadVAO;
			unsigned int quadVBO;
			std::shared_ptr<PostProcess>	shaderFbo;
			/*  private constructor*/
			Engine(void); 
			/*	private variables	*/
			static Engine                       _inst;
			std::list<std::shared_ptr<MeshRenderer>>           _meshRenderers;
			std::list<std::shared_ptr<Framebuffer>>			_framebuffers;
			std::list<std::shared_ptr<Engine42::IGameObject>>   _gameObjs;
			SDL_Event                           _event;
			const Uint8                         *_keys;
			const SdlWindow                     *_win;
			std::shared_ptr<Skybox>             _skybox;       
			/*	private functions	*/
			void                                _RenderAll(void);
			void                                _UpdateAll(void);
			void                                _FixedUpdateAll(void);

	};
}

#endif
