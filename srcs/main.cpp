
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

/*std::ostream &operator<<(std::ostream &o, glm::vec3 & vec)
{
	o << "x: " << vec.x << ", y: " << vec.y << ", z: " << vec.z;
	return o;
}*/
/*
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
	Camera cam(win.GetWidth(), win.GetHeight());

	std::string path= "cube.obj";
	glm::mat4	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	Model pillar(path.c_str(), glm::scale(model, glm::vec3(7.0f, 4.0f, 4.0f)));

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
		myShader.setMat4("view", cam.GetMatView());
		myShader.setMat4("projection", cam.GetMatProj());
		myShader.setVec3("uCamPos", cam._pos);
		myShader.setVec3("uDir", cam._dir);
		myShader.setVec3("uUp", cam._up);
		myShader.setVec2("uResolution", glm::vec2(800, 400));
		myShader.setVec2("uRotation", glm::vec2(cam._pitch, cam._yaw));
		myShader.setFloat("uFov", glm::radians(45.0f));

		std::cout << cam._pos << std::endl;
		//myShader.setFloat("pitch", cam._pitch);
		//myShader.setFloat("yaw", cam._yaw);
		//myShader.setFloat("iGlobalTime", SDL_GetTicks() / 1000.0f);


		pillar.Draw(myShader);
		win.Swap();
	}
    SDL_Quit();
}*/
Shader *test_cube(MeshRenderer **render, Model **cube)
{
	std::string path = "cube.obj";
	//std::vector<const char *>	shadersPath{"shaders/vertex.glsl", "shaders/fragment.glsl"};
	std::vector<const char *>	shadersPath{"shaders/vertex.glsl", "shaders/menger.fs.glsl"};
//	std::vector<const char *>	shadersPath{"shaders/vertex.glsl", "shaders/test_sphere_raymarching.glsl"};
	std::vector<GLenum> type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	Shader	*myShader = new Shader(shadersPath, type);
	myShader->SetIsRayMarching(true);
	//glm::mat4  matModel = glm::mat4(1.0f);
	//matModel = glm::translate(matModel, glm::vec3(0.0f, 0.0f, 0.0f));
	//matModel = glm::scale(matModel, glm::vec3(7.0f, 4.0f, 4.0f));
	//(*cube) = new Model(path.c_str(), glm::scale(matModel, glm::vec3(7.0f, 4.0f, 4.0f)));
	(*cube) = new Model(path.c_str());//, glm::mat4(1.0f));
	//						position					rotation						scale
	Transform trans = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(7.0f, 4.0f, 4.0f)};
	*render = new MeshRenderer(**cube, *myShader, trans);
	//(*render)->transform.position = glm::vec3(-3.5f, 5.5f, -0.5f);
//	(*render)->transform.position = glm::vec3(0, 0, 0);
	Engine42::Engine::AddMeshRenderer(*render);
	return myShader;
}

bool InitModels(SdlWindow &win)
{
	std::vector<const char *>	shadersPath{"shaders/vertex.glsl", "shaders/base_fragment.glsl"};
	std::vector<GLenum> type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	Shader	myShader = Shader(shadersPath, type);
	Camera cam(win.GetWidth(), win.GetHeight());

	Engine42::Engine::SetWindow(&win);
	Engine42::Engine::AddGameObject(&cam);	
	std::string path= "Pillar/LP_Pillar_Textured.obj";
	Model pillar(path.c_str());//, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
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
	MeshRenderer *render;
	Model *cube;
	Transform transform;
	transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	for (auto pos : positions)
	{
		transform.position = pos;
		render = new MeshRenderer(pillar, myShader, transform);
		//render->transform.position = pos;
		Engine42::Engine::AddMeshRenderer(render);
	}
	Shader *test = test_cube(&render, &cube);
	Engine42::Engine::Loop();
	delete test;
	//delete cube;
	delete render;
	return true;
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
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glViewport(0, 0, 800, 400);
	InitModels(win);
	//game_loop(win);
	SDL_Quit();
}
