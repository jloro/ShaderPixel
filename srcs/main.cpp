
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
#include "Skybox.hpp"
#include "PrintGlm.hpp"

void	raymarche_cube(Model &model, const Transform &trans, std::vector<const char *> shadersPath, bool useNoise = false, bool usePillar = false, float pillarOffset = 8.0f, Model *pillar = nullptr, Shader *shaderPillar = nullptr)
{
	std::vector<GLenum> type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	Shader	*myShader = new Shader(shadersPath, type);
	myShader->SetIsRayMarching(true);
	Engine42::Engine::AddMeshRenderer(new MeshRenderer(model, myShader, trans, useNoise));
	if (usePillar)
		Engine42::Engine::AddMeshRenderer(new MeshRenderer(*pillar, shaderPillar, Transform(glm::vec3(trans.position.x, trans.position.y - pillarOffset, trans.position.z))));
}
Skybox *CreateSkyBox()
{
	std::vector<std::string>	texturesPath{
	"ressources/textures/craterlake_ft.tga",
	"ressources/textures/craterlake_bk.tga",
	"ressources/textures/craterlake_up.tga",
	"ressources/textures/craterlake_dn.tga",
	"ressources/textures/craterlake_rt.tga",
	"ressources/textures/craterlake_lf.tga", 
	};
	std::vector<const char *>	shadersPath{"shaders/skybox.vs.glsl", "shaders/skybox.fs.glsl"};
	std::vector<GLenum> type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
	Skybox *skybox = new Skybox(texturesPath, shadersPath, type);
	return skybox;
}

bool InitModels(SdlWindow &win)
{
	std::vector<const char *>	shadersPath{"shaders/Vertex.vs.glsl", "shaders/Assimp.fs.glsl"};
	std::vector<GLenum>			type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

	Shader	*myShader = new Shader(shadersPath, type);
	Camera cam(win.GetWidth(), win.GetHeight());

	Engine42::Engine::SetWindow(&win);
	Engine42::Engine::AddGameObject(&cam);
	std::string path= "ressources/obj/Pillar/LP_Pillar_Textured.obj";
	Model *pillar = new Model(path.c_str());//, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	Model cube = Model("ressources/obj/cube.obj");
	path = "ressources/obj/frame/10305_picture_frame_V2_max2011_it2.obj";
	Model	frame(path.c_str());													//position						rotation					scale		
	Engine42::Engine::AddMeshRenderer(new MeshRenderer(frame, myShader, Transform(glm::vec3(0.0f, 6.3f, 0.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f))));
	Terrain terrain(10, 10, "ressources/textures/grass.png", 1, 1);
	Model *terrainModel = &terrain;
	MeshRenderer *terrainRenderer = new MeshRenderer((*terrainModel), myShader, Transform(glm::vec3(-50.0f, -7.5f, -50.0f)));
	Engine42::Engine::AddMeshRenderer(terrainRenderer);
	std::vector<const char *>	shadersPath2{"shaders/Vertex.vs.glsl", "shaders/window.fs.glsl"};
	raymarche_cube(cube, Transform(glm::vec3(0.0f, 8.1f, 0.0f),glm::vec3(1.4f, 1.9f, 0.0f)), shadersPath2);
	Transform trans(glm::vec3(0.0f, 0.0f, 0.0f),//position
						glm::vec3(4.0f, 4.0f, 4.0f));//scale
	shadersPath2[1] = "shaders/Mandelbulb.fs.glsl";
	raymarche_cube(cube, trans, shadersPath2, false, true, 8.0f, pillar, myShader);
	shadersPath2[1] = "shaders/mandelbox.fs.glsl";
	trans.position[0] = -8.0f;
	raymarche_cube(cube, trans, shadersPath2, false, true, 8.0f, pillar, myShader);
	shadersPath2[1] = "shaders/Menger.fs.glsl";
	trans.position[0] = 8.0f;
	raymarche_cube(cube, trans, shadersPath2, false, true, 8.0f, pillar, myShader);
	shadersPath2[1] = "shaders/PrettyCloud.fs.glsl";
	trans.position = glm::vec3(0.0f, 0.0f, -10.0f);
	raymarche_cube(cube, trans, shadersPath2, true, true, 8.0f, pillar, myShader);
	shadersPath2[1] = "shaders/CloudLight.fs.glsl";
	trans.position = glm::vec3(8.0f, 0.0f, -10.0f);
	raymarche_cube(cube, trans, shadersPath2, true, true, 8.0f, pillar, myShader);
	shadersPath2[1] = "shaders/PlanetMoving.fs.glsl";
	trans.position = glm::vec3(-8.0f, 0.0f, -10.0f);
	raymarche_cube(cube, trans, shadersPath2, false, true, 8.0f, pillar, myShader);
	shadersPath2[1] = "shaders/Marble.fs.glsl";
	trans.position = glm::vec3(-8.0f, 0.0f, -15.0f);
	raymarche_cube(cube, trans, shadersPath2, false, true, 8.0f, pillar, myShader);
	Skybox *sky = CreateSkyBox();
	Engine42::Engine::SetSkybox(sky);
	Engine42::Engine::Loop();
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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glViewport(0, 0, 800, 400);
	InitModels(win);
	SDL_Quit();
}
