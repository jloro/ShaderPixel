#include "Terrain.hpp"

Terrain::Terrain(unsigned int x, unsigned int z)
{
    _meshes.push_back(_GenerateTerrain(x, z));
}

Terrain::Terrain(Terrain const & src) 
{
    *this = src;
}

Terrain::~Terrain(void) {}

Terrain &	Terrain::operator = (Terrain const & rhs)
{
    static_cast<Model>(*this) = static_cast<Model>(rhs);
    return *this;
}
void        Terrain::Draw(const Shader &shader)
{
    Model::Draw(shader);
}
Mesh       Terrain::_GenerateTerrain(unsigned int x, unsigned int z)
{
    int count = _nbVertex * _nbVertex;
    float vertices[count * 3];// = new float[count * 3];
    //float[] normals = new float[count * 3];
    float textureCoords[count * 2];// = new float[count * 2];
    int indices[6 * (_nbVertex - 1) * (_nbVertex - 1)]; //= new int[6*(_nbVertex-1)*(_nbVertex-1)];
    int index = 0;
    for(int i = 0; i < _nbVertex; i++)
    {
        for(int j = 0; j < _nbVertex; j++)
        {
            vertices[index * 3] = (float)j / ((float)_nbVertex - 1) * _size;
            vertices[index * 3 + 1] = 0;
            vertices[index * 3 + 2] = (float)i / ((float)_nbVertex - 1) * _size;
            /* normals[index * 3] = 0;
            normals[index * 3 + 1] = 1;
            normals[index * 3 + 2] = 0;*/
            textureCoords[index * 2] = (float)j / ((float)_nbVertex - 1);
            textureCoords[index * 2 + 1] = (float)i / ((float)_nbVertex - 1);
            index++;
        }
    }
    index = 0;
    for(int gz = 0; gz < _nbVertex-1; gz++)
    {
        for(int gx = 0; gx < _nbVertex-1; gx++)
        {
            int topLeft = (gz * _nbVertex) + gx;
            int topRight = topLeft + 1;
            int bottomLeft = ((gz + 1) * _nbVertex) + gx;
            int bottomRight = bottomLeft + 1;
            indices[index++] = topLeft;
            indices[index++] = bottomLeft;
            indices[index++] = topRight;
            indices[index++] = topRight;
            indices[index++] = bottomLeft;
            indices[index++] = bottomRight;
        }
    }
		//return loader.loadToVAO(vertices, textureCoords, normals, indices);
}

bool    Terrain::LoadTexture(const std::string & path) 
{
    std::string dir = path.substr(0, path.find_last_of('/'));
    std::string filename = 
    Texture texture;
    texture.id = Model::_TextureFromFile(filename.c_str(), Model::_dir);
	texture.type = typeName;
	textures.push_back(texture);
    
    return true;
}   