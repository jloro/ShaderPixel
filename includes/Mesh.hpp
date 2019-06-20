#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#ifndef MESH_HPP
# define MESH_HPP

# include <iostream>

class Mesh {

public: 

    Mesh(void); 
    Mesh(Mesh const & src); 
    ~Mesh(void); 

	Mesh &		operator=(Mesh const & rhs);
private:

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