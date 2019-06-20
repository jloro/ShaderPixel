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
    static eTextureType GetEnumType(std::string type);
    static std::string GetStringType(eTextureType type);
};
#endif // TEXTURE_HPP
