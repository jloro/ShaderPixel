
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
#include <iterator>
#include "PrintGlm.hpp"

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
	*render = new MeshRenderer(**cube, *myShader, trans, true);
	Engine42::Engine::AddMeshRenderer(*render);
	return myShader;
}

Shader *raymarche_cube(MeshRenderer **render, Model **cube, Transform trans, std::string path, std::vector<const char *> shadersPath)
{
	std::vector<GLenum> type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	Shader	*myShader = new Shader(shadersPath, type);
	myShader->SetIsRayMarching(true);
	(*cube) = new Model(path.c_str());
	*render = new MeshRenderer(**cube, *myShader, trans, true);
	Engine42::Engine::AddMeshRenderer(*render);
	return myShader;
}

	template<typename T>
void	freeList(typename std::vector<T>::iterator beg, typename std::vector<T>::iterator end)
{
	for (;beg != end;beg++)
	{
		delete *beg;
	}
}

bool InitModels(SdlWindow &win)
{
	std::vector<Shader*>		shaders;
	std::vector<Model*>			models;
	std::vector<MeshRenderer*>	meshRenderer;
	std::vector<const char *>	shadersPath{"shaders/vertex.glsl", "shaders/base_fragment.glsl"};
	std::vector<GLenum>			type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

	Shader	myShader = Shader(shadersPath, type);
	Camera cam(win.GetWidth(), win.GetHeight());

	Engine42::Engine::SetWindow(&win);
	Engine42::Engine::AddGameObject(&cam);
	std::string path= "Pillar/LP_Pillar_Textured.obj";
	//Model pillar(path.c_str());//, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	MeshRenderer *render;
	Model *cube;
	Transform transform;
	transform.scale = glm::vec3(1.0f, 1.0f, 1.0f);
	transform.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	transform.position = glm::vec3(0.0f, -8.0f, 0.0f);
	//meshRenderer.push_back(new MeshRenderer(pillar, myShader, transform));
	//Engine42::Engine::AddMeshRenderer(meshRenderer.back());
	Transform trans = {glm::vec3(0.0f, 0.0f, 0.0f),//position
		glm::vec3(0.0f, 0.0f, 0.0f),//rotation
		glm::vec3(6.f, 6.f, 6.f)};//scale
	std::vector<const char *>	shadersPath2{"shaders/vertex.glsl", "shaders/CloudLight.fs.glsl"};
	shaders.push_back(raymarche_cube(&render, &cube, trans, "cube.obj", shadersPath2));
	Engine42::Engine::Loop();
	freeList<Shader*>(shaders.begin(), shaders.end());
	freeList<MeshRenderer*>(meshRenderer.begin(), meshRenderer.end());
	delete cube;
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
	glEnable(GL_CULL_FACE);  
	glCullFace(GL_FRONT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glViewport(0, 0, 800, 400);
	InitModels(win);
	//game_loop(win);
	SDL_Quit();
}
