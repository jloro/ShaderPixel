
#include "Skybox.hpp"
#include <iostream>

Skybox::Skybox(const std::vector<std::string> &texFilenames,  const std::vector< const char *> &shadersFilenames, 
const std::vector<GLenum> &shaderType)
{

    Texture texture;
    _shader = new Shader(shadersFilenames, shaderType);
    _cubeMap = _LoadCubeMap(texFilenames);
    texture.id = _cubeMap;
    texture.type =  eTextureType::Cubemap;
    _LoadModel("ressources/obj/cube.obj");
    if (_meshes.size() == 0)
    {
        throw std::runtime_error("Skybox runmesh error");
    }
    _meshes[0].textures.push_back(texture);
    _meshes[0].SendToOpenGL();
}

/*Skybox::Skybox(Skybox const & src) 
{
    *this = src;
}*/

Skybox::~Skybox(void)
{
    if (_shader != nullptr)
        delete _shader;   
}
void    Skybox::Draw(void) const
{
	glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL); 
    glDepthMask(GL_FALSE);
    _shader->use();
    _shader->setMat4("view",glm::mat4(glm::mat3(Camera::instance->GetViewMatrix())));  
    _shader->setMat4("projection", Camera::instance->GetMatProj());
    _meshes[0].Draw(*_shader);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
}
void    Skybox::Draw(const Shader &shader) const
{
    _model->Draw(shader);
}
/* Skybox &	Skybox::operator=(Skybox const & rhs)
{
    return *this;
}*/

unsigned int Skybox::_LoadCubeMap(const std::vector<std::string> &filenames)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < filenames.size(); i++)
    {
        unsigned char *data = stbi_load(filenames[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << filenames[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
