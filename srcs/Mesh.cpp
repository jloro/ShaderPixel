#include "Mesh.hpp"
#include <iostream>

Mesh::Mesh(void)
{
    _vao = _vbo = _ebo = 0;
}
Mesh::Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind, 
std::vector<Texture> texts)
{
    this->vertices = vert;
    this->indices = ind;
    this->textures = texts;
    this->_SetupMesh();
}
Mesh::Mesh(Mesh const & src) 
{
    _ebo = 0;
    _vbo = 0;
    _vao = 0;
    *this = src;
}

Mesh::~Mesh(void)
{
    glDeleteBuffers(1, &_ebo);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_vao);
}
void Mesh::Draw(const Shader &shader)const
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name;
        if (this->textures[i].type == eTextureType::diffuse)
        {
            name = "texture_diffuse" + std::to_string(diffuseNr++);
        }
        else if (this->textures[i].type == eTextureType::specular)
        {
            name = "texture_specular" + std::to_string(specularNr++);
        }
        shader.setFloat(("material." + name).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void Mesh::_SetupMesh()
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);
  
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glBindVertexArray(0);
}
Mesh &	Mesh::operator=(Mesh const & rhs)
{
    this->vertices = rhs.vertices;
    this->indices = rhs.indices;
    this->textures = rhs.textures;
    glDeleteBuffers(1, &_ebo);
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_vao);
    this->_SetupMesh();
    return *this;
}