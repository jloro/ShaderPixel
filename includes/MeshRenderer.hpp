#ifndef MESHRENDERER_HPP
# define MESHRENDERER_HPP

# include <iostream>
# include "Model.hpp"
# include "Camera.hpp"
# include "Transform.hpp"
# include "Time.hpp"
# include <memory>

class MeshRenderer 
{
public: 
/*	constructors / destrucors	*/
    MeshRenderer(std::shared_ptr<Model> model, std::shared_ptr<Shader>  shader, bool useNoise = false); 
    MeshRenderer(std::shared_ptr<Model> model, std::shared_ptr<Shader>  shader, const Transform &transform, bool useNoise = false); 
    MeshRenderer(const MeshRenderer & src); 
    virtual ~MeshRenderer(void); 

/*	public variables	*/
    Transform           transform;
/*	public functions	*/
    virtual glm::mat4       GetModelMatrix(void) const;
    virtual void            SetModelMatrix(glm::mat4 matrix);
    virtual void            UpdateMatrix(void);
    std::shared_ptr<Shader> GetShader(void) const;
    void                    Destroy(void);
    void                    SetShader(std::shared_ptr<Shader>  shader);
	MeshRenderer &		    operator=(const MeshRenderer & rhs);
    virtual void        Draw() const;
protected:
    std::shared_ptr<Model>     _model;
    std::shared_ptr<Shader>    _shader;
    glm::mat4                  _modelMatrix;
private:
/*	private variables	*/
	bool				_noise;
	GLuint				_noiseID;
/*	private functions	*/
	void				InitNoiseText(void);
};
#endif
