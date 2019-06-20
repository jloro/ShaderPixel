#ifndef TEXTURE_HPP
# define TEXTURE_HPP
#include <string>

enum eTextureType
{
    specular,
    diffuse,
    maxValue
};

struct Texture
{
    unsigned int id;
    eTextureType type;
};
#endif // TEXTURE_HPP
