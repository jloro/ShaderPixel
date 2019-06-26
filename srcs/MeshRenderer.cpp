#include "MeshRenderer.hpp"
#include "gtc/matrix_transform.hpp"
#include <iostream>

MeshRenderer::MeshRenderer(Model &model, Shader &shader) : _model(model), _shader(shader)
{}
void        MeshRenderer::Draw(void) const
{
    _shader.use();
    _shader.setMat4("view", Camera::instance->GetMatView());
    _shader.setMat4("projection", Camera::instance->GetMatProj());
    _model.SetModel(glm::translate(glm::mat4(1.0f), transform.position));
    _model.Draw(_shader);
}
MeshRenderer::MeshRenderer(MeshRenderer const & src) : _model(src._model), _shader(src._shader)
{}

MeshRenderer::~MeshRenderer(void) {}

MeshRenderer &	MeshRenderer::operator=(MeshRenderer const & rhs)
{
    this->_model = rhs._model;
    this->_shader = rhs._shader;
    return *this;
}