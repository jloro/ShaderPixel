
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
#include "Terrain.hpp"

std::ostream &operator<<(std::ostream &o, glm::vec3 & vec)
{
	o << "x: " << vec.x << ", y: " << vec.y << ", z: " << vec.z;
	return o;
}

Shader *test_cube(MeshRenderer **render, Model **cube)
{
	std::string path = "cube.obj";
	std::vector<const char *>	shadersPath{"shaders/vertex.glsl", "shaders/mandelbulb.fs.glsl"};
	std::vector<GLenum> type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	Shader	*myShader = new Shader(shadersPath, type);
	myShader->SetIsRayMarching(true);
	(*cube) = new Model(path.c_str());//, glm::mat4(1.0f));
	//						position					rotation						scale
	Transform trans = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)};
	*render = new MeshRenderer(**cube, *myShader, trans);
	Engine42::Engine::AddMeshRenderer(*render);
	return myShader;
}

Shader *test_cube2(MeshRenderer **render, Model **cube)
{
	std::string path = "cube.obj";
	std::vector<const char *>	shadersPath{"shaders/vertex.glsl", "shaders/mandelbox.fs.glsl"};
	std::vector<GLenum> type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	Shader	*myShader = new Shader(shadersPath, type);
	myShader->SetIsRayMarching(true);
	(*cube) = new Model(path.c_str());//, glm::mat4(1.0f));
	//						position					rotation						scale
	Transform trans = {glm::vec3(8.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(4.0f, 4.0f, 4.0f)};
	*render = new MeshRenderer(**cube, *myShader, trans);
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
	MeshRenderer *render;
	Model *cube;
	Transform transform;
	transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	transform.position = glm::vec3(0.0f, -8.0f, 0.0f);
	render = new MeshRenderer(pillar, myShader, transform);
	Engine42::Engine::AddMeshRenderer(render);
	transform.position = glm::vec3(8.0f, -8.0f, 0.0f);
	render = new MeshRenderer(pillar, myShader, transform);
	Engine42::Engine::AddMeshRenderer(render);
	Model *terrain = new Terrain(10, 10, "textures/grass.png", 1, 1);
	MeshRenderer terrainRenderer((*terrain), myShader, Transform(glm::vec3(-50.0f, -7.5f, -50.0f)));
	Engine42::Engine::AddMeshRenderer(&terrainRenderer);
	Shader *test = test_cube(&render, &cube);
	Shader *test2 = test_cube2(&render, &cube);
	Engine42::Engine::Loop();
	delete test;
	delete test2;
	delete cube;
	delete render;
	delete terrain;
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
