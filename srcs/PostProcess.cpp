#include "PostProcess.hpp"
#include "Engine.hpp"

PostProcess::PostProcess(std::vector<const char *> shaderSource, std::vector<GLenum> shaderType) : Shader(shaderSource, shaderType), _effect(0)
{
	_ChangeKernel();
	_offset = 300.0f;
}

void	PostProcess::use(void) const
{
	glUseProgram(_program);
	setInt("uEffect", _effect);
	setMat3("uKernel", _kernel);
	setFloat("uOffset", 1 /_offset);
}

PostProcess::~PostProcess() {}

void	PostProcess::Update(void)
{
	const Uint8 	*keys = Engine42::Engine::GetKeyInput();
	if (keys[SDL_SCANCODE_0])
		_effect = 0;
	else if (keys[SDL_SCANCODE_1])
		_effect = 1;
	else if (keys[SDL_SCANCODE_2])
		_effect = 2;
	else if (keys[SDL_SCANCODE_3])
		_effect = 3;
	else if (keys[SDL_SCANCODE_4])
		_effect = 4;
	else if (keys[SDL_SCANCODE_5])
		_effect = 5;
	if (keys[SDL_SCANCODE_KP_PLUS])
		_offset += 10.0f;
	else if (keys[SDL_SCANCODE_KP_MINUS])
		_offset -= 10.0f;
	_ChangeKernel();
}
void 	PostProcess::_ChangeKernel()
{
	if (_effect < 3)
	{
		_kernel[0] = glm::vec3(0.0f,0.0f, 0.0f);
		_kernel[1] = glm::vec3(0.0f, 1.0f, 0.0f);
		_kernel[2] = glm::vec3( 0.0f, 0.0f, 0.0f);
	}
	else if (_effect == 3)
	{
		_kernel[0] = glm::vec3(-1.0f,-1.0f, -1.0f);
		_kernel[1] = glm::vec3(-1.0f,9.0f, -1.0f);
		_kernel[2] = glm::vec3(-1.0f,-1.0f, -1.0f);
	}
	else if (_effect == 4)
	{
		_kernel[0] = glm::vec3(1.0f, 2.0f, 1.0f);
		_kernel[1] = glm::vec3(0.0f, 0.0f, 0.0f);
		_kernel[2] = glm::vec3(-1.0f,-2.0f,-1.0f);
	}
	else if (_effect == 5)
	{
		_kernel[0] = glm::vec3(0.0f,-1.0f, 0.0f);
		_kernel[1] = glm::vec3(-1.0f, 5.0f, -1.0f);
		_kernel[2] = glm::vec3( 0.0f,-1.0f, 0.0f);
	}
}

void	PostProcess::FixedUpdate(void)
{

}
