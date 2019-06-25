#ifndef MESHRENDERER_HPP
# define MESHRENDERER_HPP

# include <iostream>
# include "Model.hpp"
# include "Camera.hpp"
# include "Transform.hpp"

class MeshRenderer 
{
public: 
/*	constructors / destrucors	*/
    MeshRenderer(Model &model, Shader &shader); 
    MeshRenderer(const MeshRenderer & src); 
    virtual ~MeshRenderer(void); 

/*	public variables	*/
    Transform           transform;
/*	public functions	*/
	MeshRenderer &		operator=(const MeshRenderer & rhs);
    virtual void        Draw() const;
protected:
    Model               &_model;
    Shader              &_shader;
private:
/*	private variables	*/
/*	private functions	*/
};
#endif