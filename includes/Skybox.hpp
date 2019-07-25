#ifndef SKYBOX_HPP
# define SKYBOX_HPP

# include <iostream>
# include <vector>
/* # ifndef STB_IMAGE_IMPLEMENTATION
#  define STB_IMAGE_IMPLEMENTATION
# endif*/
# include "stb_image.h"
# include "glad.h"
# include "Shader.hpp"
# include "Camera.hpp"

class Skybox 
{

public: 
/*	constructors / Destructor*/	
    Skybox(const std::vector< std::string> &texFilenames,const std::vector< const char *> &shadersFilenames, const std::vector<GLenum> &shaderType);
    //Skybox(Skybox const & src); 
    ~Skybox(void); 
/*	public variables*/	
/*	public functions*/	
    void            Draw();

    std::string const	toString(void) const;


protected:
    unsigned int _vbo;
    unsigned int _cubeMap;
/*	protected variables*/
    Shader         *_shader;
/*	protected functions*/
    void _CreateCube(void);

    static unsigned int _LoadCubeMap(const std::vector<std::string> &filenames);

};

#endif