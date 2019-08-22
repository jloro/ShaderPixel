#include "MeshRenderer.hpp"
#include "gtc/matrix_transform.hpp"
#include <iostream>
#include "stb_image.h"
#include <cstdlib>
#include <time.h>
#include "glm.hpp"
#include <gtc/random.hpp>
#include "Engine.hpp"


MeshRenderer::MeshRenderer(std::shared_ptr<Model> model, std::shared_ptr<Shader>  shader, bool useNoise) : _model(model), _shader(shader), _noise(useNoise)
{
    transform = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)};
    UpdateMatrix();
	if (useNoise)
		InitNoiseText();
}
MeshRenderer::MeshRenderer(std::shared_ptr<Model> model, std::shared_ptr<Shader>  shader, const Transform &trans, bool useNoise) : _model(model), _shader(shader), _noise(useNoise)
{
    transform = trans;
    UpdateMatrix();
	if (useNoise)
		InitNoiseText();
}
void        MeshRenderer::Draw(void) const
{
	if (_shader == nullptr)
	{
		std::cout << "MeshRenderer : Cannot Draw with whitout shader" << std::endl;
		return;
	}
    _shader->use();
    _shader->setMat4("view", Camera::instance->GetMatView());
    _shader->setMat4("projection", Camera::instance->GetMatProj());
    _shader->setMat4("model", _modelMatrix);
	_shader->setVec3("uOrigin", transform.position);
	if (_noise)
	{
		glActiveTexture(GL_TEXTURE10);
		_shader->setInt("uNoise", 10);
		glBindTexture(GL_TEXTURE_2D, _noiseID);
	}
    if (_shader->GetIsRayMarching())
	{
        _shader->SetUpUniforms(*Camera::instance, *SdlWindow::GetMain(), ((float)SDL_GetTicks()) / 1000.f);
		glCullFace(GL_FRONT);
	}
	else
		glCullFace(GL_BACK);
	_model->Draw(_shader);
}

glm::mat4       MeshRenderer::GetModelMatrix(void) const {return _modelMatrix;}
void            MeshRenderer::SetModelMatrix(glm::mat4 matrix) {_modelMatrix = matrix;}

void            MeshRenderer::UpdateMatrix(void) 
{
    _modelMatrix = glm::mat4(1.0f);
    _modelMatrix = glm::translate(_modelMatrix, transform.position);
    _modelMatrix = glm::rotate(_modelMatrix, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    _modelMatrix = glm::rotate(_modelMatrix, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    _modelMatrix = glm::rotate(_modelMatrix, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    _modelMatrix = glm::scale(_modelMatrix, transform.scale);
}
MeshRenderer::MeshRenderer(MeshRenderer const & src) : _model(src._model), _shader(src._shader)
{}

MeshRenderer::~MeshRenderer(void) {}

MeshRenderer &	MeshRenderer::operator=(MeshRenderer const & rhs)
{
    this->_model = rhs._model;
    this->_shader = rhs._shader;
    this->transform = rhs.transform;
    UpdateMatrix();
    return *this;
}

struct color
{
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
	unsigned char	a;
};

color *	genNoiseTex()
{
	srand (time(NULL));
	color * tex = (color *)malloc(sizeof(unsigned char) * 256 * 256 * 4);
	for (unsigned int y = 0; y < 256; y++)
	{
		for (unsigned int x = 0; x < 256; x++)
		{
			int idx = y * 256 + x;
			tex[idx].r = rand() % 255;
			tex[idx].b = rand() % 255;
		}
	}
	
	for (unsigned int y = 0; y < 256; y++)
	{
		for (unsigned int x = 0; x < 256; x++)
		{
			int x2 = (x - 37) & 255;
			int y2 = (y - 17) & 255;
			unsigned int idx2 = y2 * 256 + x2;
			unsigned int idx = y * 256 + x;
			tex[idx].g = tex[idx2].r;
			tex[idx].a = tex[idx2].b;
		}
	}
	return tex;
}

void		MeshRenderer::InitNoiseText(void)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	color *data = genNoiseTex();
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	_noiseID = textureID;
	free(data);
}
void MeshRenderer::Destroy(void)
{
	Engine42::Engine::Destroy(std::shared_ptr<MeshRenderer>(this));
}
void MeshRenderer::SetShader(std::shared_ptr<Shader>  shader)
{
	_shader = shader;
}

std::shared_ptr<Shader> MeshRenderer::GetShader(void) const
{
	return _shader;
}
