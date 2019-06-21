
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

void			game_loop(SdlWindow &win)
{
	bool	quit = false;
	SDL_Event	event;

	glEnable(GL_DEPTH_TEST);
	std::vector<const char *>	shadersPath{"shaders/vertex.glsl", "shaders/fragment.glsl"};
	std::vector<GLenum> type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	Shader	myShader(shadersPath, type);
	Camera cam(800, 600, 0, 0);

	std::string path= "nanosuit/nanosuit.obj";
	Model crysis(path.c_str());
	while (!quit)
	{
		SDL_WaitEvent(&event);

		if(event.window.event == SDL_WINDOWEVENT_CLOSE)
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
	SdlWindow	win(800, 400, false, true, "test");
	win.CreateGlContext(4, 1, true, 24);
	game_loop(win);
}
