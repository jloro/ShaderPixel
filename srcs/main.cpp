
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
	std::vector<const char *>	shadersPath{"shaders/vertex.glsl", "shaders/fragment.glsl"};
	std::vector<GLenum> type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	Shader	myShader(shadersPath, type);
	int	mouseX, mouseY;
	SDL_GetRelativeMouseState(&mouseX, &mouseY);
	Camera cam(800, 400, mouseX, mouseY);

	std::string path= "nanosuit/nanosuit.obj";
	Model crysis(path.c_str());
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

		myShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		myShader.setMat4("model", model);
		myShader.setMat4("view", cam.GetMatView());
		myShader.setMat4("projection", cam.GetMatProj());

		crysis.Draw(myShader);
		win.Swap();
	}
    SDL_Quit();
}

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
	game_loop(win);
}
