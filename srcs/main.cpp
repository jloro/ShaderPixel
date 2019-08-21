
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
#include "Framebuffer.hpp"

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
	Model *pillar, *cube, *frame;
	try {
		cube = new Model("ressources/obj/cube.obj");
		pillar = new Model("ressources/obj/Pillar/LP_Pillar_Textured.obj");
		frame = new Model("ressources/obj/frame/10305_picture_frame_V2_max2011_it2.obj");
	} catch (std::runtime_error & e) {
		std::cout << e.what() << std::endl;
		return false;
	}

	std::vector<const char *>	shadersPath{"shaders/fbo.vs.glsl", "shaders/fbo.fs.glsl"};
	std::vector<GLenum>			type{GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};

	Shader	*shaderfbo = new Shader(shadersPath, type);
	Camera cam(win.GetWidth(), win.GetHeight());

	shadersPath[0] = "shaders/Vertex.vs.glsl";
	shadersPath[1] = "shaders/Assimp.fs.glsl";
	Shader	*myShader = new Shader(shadersPath, type);
	Transform trans(glm::vec3(0.0f, 0.1f, -30.0f),//position
						glm::vec3(1.4f, 1.9f, 0.0f));//scale

	Engine42::Engine::SetWindow(&win);
	Engine42::Engine::AddGameObject(&cam);
	Engine42::Engine::AddFramebuffer(new Framebuffer(win.GetWidth(), win.GetHeight(), shaderfbo, *cube, trans));
	Engine42::Engine::AddMeshRenderer(new MeshRenderer(*frame, myShader, Transform(glm::vec3(0.0f, -1.7f, -20.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f))));
	Engine42::Engine::AddMeshRenderer(new MeshRenderer(*frame, myShader, Transform(glm::vec3(0.0f, -1.7f, -30.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f))));
	Terrain terrain(10, 10, "ressources/textures/grass.png", 1, 1);
	Model *terrainModel = &terrain;
	MeshRenderer *terrainRenderer = new MeshRenderer((*terrainModel), myShader, Transform(glm::vec3(-50.0f, -7.5f, -50.0f)));
	Engine42::Engine::AddMeshRenderer(terrainRenderer);
	std::vector<const char *>	shadersPath2{"shaders/Vertex.vs.glsl", "shaders/Window.fs.glsl"};
	raymarche_cube(*cube, Transform(glm::vec3(0.0f, 0.1f, -20.0f),glm::vec3(1.4f, 1.9f, 0.0f)), shadersPath2);
	trans.position = glm::vec3(0.0f, 0.0f, 0.0f);
	trans.scale = glm::vec3(4.0f, 4.0f, 4.0f);
	shadersPath2[1] = "shaders/mandelbulb.fs.glsl";
	raymarche_cube(*cube, trans, shadersPath2, false, true, 8.0f, pillar, myShader);
	shadersPath2[1] = "shaders/mandelbox.fs.glsl";
	trans.position[0] = -10.0f;
	raymarche_cube(*cube, trans, shadersPath2, false, true, 8.0f, pillar, myShader);
	shadersPath2[1] = "shaders/Menger.fs.glsl";
	trans.position[0] = 10.0f;
	raymarche_cube(*cube, trans, shadersPath2, false, true, 8.0f, pillar, myShader);
	shadersPath2[1] = "shaders/PrettyCloud.fs.glsl";
	trans.position = glm::vec3(0.0f, 0.0f, -10.0f);
	raymarche_cube(*cube, trans, shadersPath2, true, true, 8.0f, pillar, myShader);
	shadersPath2[1] = "shaders/CloudLight.fs.glsl";
	trans.position = glm::vec3(10.0f, 0.0f, -10.0f);
	raymarche_cube(*cube, trans, shadersPath2, true, true, 8.0f, pillar, myShader);
	shadersPath2[1] = "shaders/PlanetMoving.fs.glsl";
	trans.position = glm::vec3(-10.0f, 0.0f, -10.0f);
	raymarche_cube(*cube, trans, shadersPath2, false, true, 8.0f, pillar, myShader);
	shadersPath2[1] = "shaders/Marble.fs.glsl";
	trans.position = glm::vec3(-10.0f, 0.0f, -20.0f);
	raymarche_cube(*cube, trans, shadersPath2, false, true, 8.0f, pillar, myShader);
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
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	InitModels(win);
	SDL_Quit();
}
