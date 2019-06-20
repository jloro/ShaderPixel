#ifndef VERTEX_HPP
# define VERTEX_HPP
# include "glm.hpp"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};
#endif