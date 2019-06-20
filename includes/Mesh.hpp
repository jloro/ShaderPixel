#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#ifndef MESH_HPP
# define MESH_HPP
# include "Vertex.hpp"
# include "Texture.hpp"

# include <iostream>
# include <vector>

class Mesh 
{

public: 
/*  constructors / destructors  */
    Mesh(void); 
    Mesh(vector<Vertex> vert, vector<unsigned int> ind, vector<Texture> texts); 
    Mesh(Mesh const & src); 
    ~Mesh(void); 
/*  public variables */
    vector<Vertex>          vertices;
    vector<unsigned int>    indices;
    vector<Texture>         textures;
/*  public functions  */
    void        Draw(Shader shader);
	Mesh &		operator=(Mesh const & rhs);
private:
/*  private variables  */
    unsigned int _vao;
    unsigned int _vbo;
    unsigned int _ebo;

/*  private functions */
    void setupMesh(void);
};

#endif


#include "Mesh.hpp"
#include <iostream>

Mesh::Mesh(void)
{
    
}

Mesh::Mesh(Mesh const & src) 
{
    *this = src;
    return;
}

Mesh::~Mesh(void)
{
    
}

Mesh &	Mesh::operator=(Mesh const & rhs)
{
    return *this;
}