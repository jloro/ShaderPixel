
#include "SdlWindow.hpp"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model.hpp"
#include "Engine.hpp"

std::ostream &operator<<(std::ostream &o, glm::vec3 & vec)
{
	o << "x: " << vec.x << ", y: " << vec.y << ", z: " << vec.z;
	return o;
}

void			processInput(const Uint8 *state, bool& quit, float deltaTime)
{
	if (state[SDL_SCANCODE_ESCAPE])
		quit = true;
	if (state[SDL_SCANCODE_W])
		Camera::instance->Move(eCameraDirection::Forward, deltaTime);
	if (state[SDL_SCANCODE_S])
		Camera::instance->Move(eCameraDirection::Backward, deltaTime);
	if (state[SDL_SCANCODE_D])
		Camera::instance->Move(eCameraDirection::Right, deltaTime);
	if (state[SDL_SCANCODE_A])
		Camera::instance->Move(eCameraDirection::Left, deltaTime);
	if (state[SDL_SCANCODE_LCTRL])
		Camera::instance->Move(eCameraDirection::Down, deltaTime);
	if (state[SDL_SCANCODE_SPACE])
		Camera::instance->Move(eCameraDirection::Up, deltaTime);
}

void			game_loop(SdlWindow &win)
{
	bool	quit = false;
	unsigned int		last_time = SDL_GetTicks();
	unsigned int		delta = 0.0f;
	const unsigned int fixdelta = 20;
	const Uint8 *state;
	SDL_Event e;

	glEnable(GL_DEPTH_TEST);
	std::vector<const char *>	shadersPath{"shaders/vertex.glsl", "shaders/fragmentAssimp.glsl"};
	std::vector<const char *>	shadersPath2{"shaders/vertex.glsl", "shaders/fragmentRayMarche.glsl"};
	std::vector<GLenum> type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	Shader	raymarcheShader(shadersPath2, type);
	Shader	obj(shadersPath, type);
	Camera cam(win.GetWidth(), win.GetHeight());

	std::string path= "cube.obj";
	glm::mat4	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	Model raymarche(path.c_str(), glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f)));
	Model pillar("Pillar/LP_Pillar_Textured.obj", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -9.5f, 0.0f)));

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
			if (e.type == SDL_MOUSEMOTION)
				cam.LookAround(e.motion.xrel, -e.motion.yrel);
		state = SDL_GetKeyboardState(NULL);

		delta += SDL_GetTicks() - last_time;
		if (delta >= fixdelta)
		{
			processInput(state, quit, 0.01f);
			delta = 0.0f;
		}
		last_time = SDL_GetTicks();

		if(e.window.event == SDL_WINDOWEVENT_CLOSE)
			quit = true;

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		obj.use();
		obj.setMat4("view", cam.GetMatView());
		obj.setMat4("projection", cam.GetMatProj());

		pillar.Draw(obj);

		raymarcheShader.use();
		raymarcheShader.setMat4("view", cam.GetMatView());
		raymarcheShader.setMat4("projection", cam.GetMatProj());
		raymarcheShader.setVec3("uCamPos", cam._pos);
		raymarcheShader.setVec3("uDir", cam._dir);
		raymarcheShader.setVec3("uUp", cam._up);
		raymarcheShader.setVec2("uResolution", glm::vec2(800, 400));
		raymarcheShader.setVec2("uRotation", glm::vec2(cam._pitch, cam._yaw));
		raymarcheShader.setFloat("uFov", glm::radians(45.0f));
		raymarcheShader.setFloat("uGlobalTime", SDL_GetTicks() / 1000.0f);

		raymarche.Draw(raymarcheShader);
		win.Swap();
	}
    SDL_Quit();
}
/*
bool InitModels(SdlWindow &win)
{
	glEnable(GL_DEPTH_TEST);
	std::vector<const char *>	shadersPath{"shaders/vertex.glsl", "shaders/fragment.glsl"};
	std::vector<GLenum> type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	Shader	myShader = Shader(shadersPath, type);
	Camera cam(win.GetWidth(), win.GetHeight());

	Engine42::Engine::SetWindow(&win);
	Engine42::Engine::AddGameObject(&cam);	
	std::string path= "Pillar/LP_Pillar_Textured.obj";
	Model pillar(path.c_str(), glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	glm::vec3 positions[] = {
		glm::vec3(-6.0f, 0.0f, 0.0f),
		glm::vec3(-3.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(3.0f, 0.0f, 0.0f),
		glm::vec3(6.0f, 0.0f, 0.0f),
		glm::vec3(-6.0f, 0.0f, 3.0f),
		glm::vec3(-3.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(3.0f, 0.0f, 3.0f),
		glm::vec3(6.0f, 0.0f, 3.0f),
	};
	for (auto pos : positions)
	{
		MeshRenderer *render = new MeshRenderer(pillar, myShader);
		render->transform.position = pos;
		Engine42::Engine::AddMeshRenderer(render);
	}
	Engine42::Engine::Loop();
	return true;
}
*/
int				main(int ac, char **av)
{
	if (ac < -1 && av == nullptr)
		return 1;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Erreur lors de l'initialisation de la SDL : " << std::endl;
		std::cout << SDL_GetError() << std::endl;;
		SDL_Quit();
		return (EXIT_SUCCESS);
	}
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SdlWindow	win(800, 400, false, true, "test");
	win.CreateGlContext(4, 1, true, 24);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glViewport(0, 0, 800, 400);
	//InitModels(win);
	game_loop(win);
	SDL_Quit();
}