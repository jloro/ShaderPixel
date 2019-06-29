#ifndef MESHRENDERER_HPP
# define MESHRENDERER_HPP

# include <iostream>
# include "Model.hpp"
# include "Camera.hpp"
# include "Transform.hpp"
# include "Time.hpp"

class MeshRenderer 
{
public: 
/*	constructors / destrucors	*/
    MeshRenderer(Model &model, Shader &shader); 
    MeshRenderer(Model &model, Shader &shader, const Transform &transform); 
    MeshRenderer(const MeshRenderer & src); 
    virtual ~MeshRenderer(void); 

/*	public variables	*/
    Transform           transform;
/*	public functions	*/
    glm::mat4           GetModelMatrix(void) const;
    void                SetModelMatrix(glm::mat4 matrix);
    void                UpdateMatrix(void);
	MeshRenderer &		operator=(const MeshRenderer & rhs);
    virtual void        Draw() const;
protected:
    Model               &_model;
    Shader              &_shader;
    glm::mat4           _modelMatrix;
private:
/*	private variables	*/
/*	private functions	*/
};
#endif