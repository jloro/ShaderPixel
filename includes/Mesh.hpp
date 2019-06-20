#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#ifndef MESH_HPP
# define MESH_HPP
# include "Vertex.hpp"
# include "Texture.hpp"
# include "Shader.hpp"

# include <iostream>
# include <vector>
# include "assimp/Importer.hpp"
# include "assimp/scene.h"
# include "assimp/postprocess.h"

class Mesh 
{

public: 
/*  constructors / destructors  */
    Mesh(void); 
    Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind, std::vector<Texture> texts); 
    Mesh(Mesh const & src); 
    ~Mesh(void); 
/*  public variables */
    std::vector<Vertex>          vertices;
    std::vector<unsigned int>    indices;
    std::vector<Texture>         textures;
/*  public functions  */
    void        Draw(const Shader &shader) const;
	Mesh &		operator=(Mesh const & rhs);
private:
/*  private variables  */
    unsigned int _vao;
    unsigned int _vbo;
    unsigned int _ebo;

/*  private functions */
    void _SetupMesh(void);
};

#endif