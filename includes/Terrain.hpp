#ifndef TERRAIN_HPP
# define TERRAIN_HPP

# include "Model.hpp"

class Terrain : public Model
{

public: 
/*  constructors / Destructor   */
    Terrain(unsigned int x, unsigned int z); 
    Terrain(Terrain const & src); 
    ~Terrain(void); 
/*  functions   */
    virtual void Draw(const Shader &shader);
    virtual bool LoadTexture(const std::string &filename);
	Terrain &		operator=(Terrain const & rhs);

private:
/*  variables   */
    static const unsigned int _size;
    static const unsigned int _nbVertex;
/*  functions   */
	Mesh       _GenerateTerrain(unsigned int x, unsigned int z);
};

#endif


