#ifndef TRANSFORM_HPP
# define TRANSFORM_HPP

# include <glm.hpp>

struct Transform
{
    public :
    glm::vec3	position;
    glm::vec3	rotation;
    glm::vec3	scale;
    Transform();
    Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
    Transform(glm::vec3 pos, glm::vec3 scale);
    Transform(glm::vec3 pos);
    private:
        void    _Initialize(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
};
#endif
