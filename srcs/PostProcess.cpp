#include "PostProcess.hpp"
#include "Engine.hpp"

PostProcess::PostProcess(std::vector<const char *> shaderSource, std::vector<GLenum> shaderType) : Shader(shaderSource, shaderType), _effect(0)
{}

void	PostProcess::use(void) const
{
	glUseProgram(_program);
	setInt("uEffect", _effect);
}

PostProcess::~PostProcess() {}

void	PostProcess::Update(void)
{
	const Uint8 	*keys = Engine42::Engine::GetKeyInput();
	if (keys[SDL_SCANCODE_0])
		_effect = 0;
	if (keys[SDL_SCANCODE_1])
		_effect = 1;
}

void	PostProcess::FixedUpdate(void)
{

}
