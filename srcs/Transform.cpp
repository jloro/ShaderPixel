    #include "Transform.hpp"

    Transform::Transform()
    {
        _Initialize(glm::vec3(0, 0, 0), glm::vec3(0,0,0), glm::vec3(1,1,1));
    }
    Transform::Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
    {
        _Initialize(pos, rot, scale);
    }
    Transform::Transform(glm::vec3 pos, glm::vec3 scale)
    {
        _Initialize(pos, glm::vec3(0,0,0), scale);
    }
    Transform::Transform(glm::vec3 pos)
    {
        _Initialize(pos, glm::vec3(0,0,0), glm::vec3(1,1,1));
    }
    void    Transform::_Initialize(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
    {
        this->position = pos;
        this->rotation = rot;
        this->scale = scale;
    }