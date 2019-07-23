#include "MeshRenderer.hpp"
#include "gtc/matrix_transform.hpp"
#include <iostream>

MeshRenderer::MeshRenderer(Model &model, Shader &shader) : _model(model), _shader(shader)
{
    transform = {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)};
    UpdateMatrix();
}
MeshRenderer::MeshRenderer(Model &model, Shader &shader, const Transform &trans) : _model(model), _shader(shader)
{
    transform = trans;
    UpdateMatrix();
}
void        MeshRenderer::Draw(void) const
{
    _shader.use();
    _shader.setMat4("view", Camera::instance->GetMatView());
    _shader.setMat4("projection", Camera::instance->GetMatProj());
    _shader.setMat4("model", _modelMatrix);
	_shader.setVec3("uOrigin", transform.position);
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
    //glm::mat4 rot = glm::rotate(_modelMatrix, glm::radians(90.0f),transform.rotation);
    //_modelMatrix *= rot;//;glm::rotate(_modelMatrix, glm::radians(90.0f),transform.rotation);
    //_modelMatrix = glm::rotate(_modelMatrix, glm::radians(90.0f),transform.rotation);
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
