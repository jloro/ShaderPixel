#include "MeshRenderer.hpp"
#include "gtc/matrix_transform.hpp"
#include <iostream>
#include "stb_image.h"

MeshRenderer::MeshRenderer(Model &model, Shader &shader, bool useNoise) : _model(model), _shader(shader), _noise(useNoise)
{
    transform = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)};
    UpdateMatrix();
	if (useNoise)
		InitNoiseText();
}
MeshRenderer::MeshRenderer(Model &model, Shader &shader, const Transform &trans, bool useNoise) : _model(model), _shader(shader), _noise(useNoise)
{
    transform = trans;
    UpdateMatrix();
	if (useNoise)
		InitNoiseText();
}
void        MeshRenderer::Draw(void) const
{
    _shader.use();
    _shader.setMat4("view", Camera::instance->GetMatView());
    _shader.setMat4("projection", Camera::instance->GetMatProj());
    _shader.setMat4("model", _modelMatrix);
	_shader.setVec3("uOrigin", transform.position);
	if (_noise)
	{
		glActiveTexture(GL_TEXTURE10);
		_shader.setInt("uNoise", 10);
		glBindTexture(GL_TEXTURE_2D, _noiseID);
	}
    if (_shader.GetIsRayMarching())
        _shader.SetUpUniforms(*Camera::instance, *SdlWindow::GetMain(), ((float)SDL_GetTicks()) / 1000.f);
    _model.Draw(_shader);
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

void flipInPlace(unsigned char* data, int width, int height) {
   size_t line_size = width * 4;
   unsigned char* line_buffer = new unsigned char[line_size];
   int half_height = height / 2;
   for (int y = 0; y < half_height;y++) {
     void* top_line = data + y * line_size;
     void* bottom_line = data + (height - y - 1) * line_size;
     memcpy(line_buffer, top_line, line_size);
     memcpy(top_line, bottom_line, line_size);
     memcpy(bottom_line, line_buffer, line_size);
   }
   delete [] line_buffer;
 }

void		MeshRenderer::InitNoiseText(void)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load("textures/noise 512.png", &width, &height, &nrComponents, 0);
	if (data)
	{
		flipInPlace(data, width, height);
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "failed to load texture noise" << std::endl;
		stbi_image_free(data);
	}
	_noiseID = textureID;
}
