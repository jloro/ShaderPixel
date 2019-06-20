#ifndef MODEL_HPP
# define MODEL_HPP

# include <iostream>
# include "Mesh.hpp"

class Model 
{
public: 
/*  constructors / destructors*/
    Model(void); 
    Model(std::string path); 
    Model(const Model & src); 
    ~Model(void); 
/*  public variables    */
/*  public functions    */
    void                Draw(const Shader &shader);
	Model &		        operator=(const Model & rhs);
    const std::string	toString(void) const;
private:
/*  private variables   */
    std::vector<Mesh>   _meshes;
    std::string         _directory;
/*  private functions   */
    void            _LoadModel(const std::string &path);
    void            _ProcessNode(aiNode *node, const aiScene *scene);
    Mesh            _ProcessMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> _LoadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif

/*
#include "Model.hpp"
#include <iostream>

Model::Model(void)
{
    
}

Model::Model(Model const & src) 
{
    *this = src;
    return;
}

Model::~Model(void)
{
    
}

Model &	Model::operator=(Model const & rhs)
{
    return *this;
}

std::string const Model::toString(void) const
{
    
}

std::ostream &	operator<< (std::ostream & o, Model const & rhs)
{
    o << rhs.toString();
    return o;
}*/