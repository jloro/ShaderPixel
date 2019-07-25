#include "Terrain.hpp"
const unsigned int Terrain::_size = 10;

Terrain::Terrain() : Model(), _x(_size), _z(_size)//protected
{
    _meshes.push_back(Mesh());
    _dir = "";
}
Terrain::Terrain(unsigned int x, unsigned int z) : Model(), _x (x * _size), _z(z * _size)// the mesh will not send his variables to openGL
{
    _tilingX = 1.0f;
    _tilingY = 1.0f;
    _meshes.push_back(_GenerateTerrain(x, z));
}
Terrain::Terrain(unsigned int x, unsigned int z, const std::string &path) : Model(), _x (x * _size), _z(z * _size)
{
    _tilingX = 1.0f;
    _tilingY = 1.0f;
    _meshes.push_back(_GenerateTerrain(x, z));
    LoadTexture(path);
   _meshes[0].SendToOpenGL();
}
Terrain::Terrain(unsigned int x, unsigned int z, const std::string &path, float tilingX, float tilingY) : Model(), _x (x * _size), _z(z * _size)
{
    _tilingX = tilingX;
    _tilingY = tilingY;
    _meshes.push_back(_GenerateTerrain(x, z));
    LoadTexture(path);
   _meshes[0].SendToOpenGL();
}
Terrain::Terrain(Terrain const & src) : Model(src), _x(src._x), _z(src._z)
{
    *this = src;
}

Terrain::~Terrain(void) {}

Terrain &	Terrain::operator = (Terrain const & rhs)
{
    static_cast<Model>(*this) = static_cast<Model>(rhs);
    return *this;
}
Mesh       Terrain::_GenerateTerrain(unsigned int xSize, unsigned int zSize)
{
    const unsigned int nb_vert = (xSize) * (zSize);
    std::vector<unsigned int>   faces((nb_vert)* 6);
    std::vector<Vertex>     vertices;
    for (unsigned int i = 0,  z = 0; z <= zSize; z++)
    {
        for (unsigned int x = 0; x <= xSize; x++)
        {
            Vertex vert;
            vert.position.x = (static_cast<float>(x) *_size);
            vert.position.y = 0;
            vert.position.z = static_cast<float>(z) * _size;
            vert.texCoord.x = static_cast<float>(x) / _tilingX;
            vert.texCoord.y = static_cast<float>(z) / _tilingY;
            vertices.push_back(vert);
            i++;
        }
    }
    unsigned int intVert = 0;
    unsigned int tris = 0;
    for (unsigned int z = 0; z < zSize; z++)
    {
        for (unsigned int x = 0; x < xSize; x++)
        {
            faces[tris + 0] = intVert + 0;
            faces[tris + 1] = intVert + xSize +1;
            faces[tris + 2] = intVert + 1;
            faces[tris + 3] = intVert + 1;
            faces[tris + 4] = intVert + xSize + 1;
            faces[tris + 5] = intVert + xSize + 2;

            intVert++;
            tris += 6;
        }
        intVert++;
    }
    Mesh mesh = Mesh();
    mesh.vertices = vertices;
    mesh.faces = faces;
    return mesh;
}

bool    Terrain::LoadTexture(const std::string & path) 
{
    _dir  = path.substr(0, path.find_last_of('/'));
    _meshes[0].textures.push_back(Model::_LoadSimpleTexture(eTextureType::Specular ,path));
    return true;
}   
bool Terrain::LoadTexture(const std::string &path, float tilingX, float tilingY)
{
    _tilingX = tilingX;
    _tilingY = tilingY;
    _dir  = path.substr(0, path.find_last_of('/'));
    _meshes[0].textures.push_back(Model::_LoadSimpleTexture(eTextureType::Specular ,path));
    return true;
}