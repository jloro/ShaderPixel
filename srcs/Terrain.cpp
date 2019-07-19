#include "Terrain.hpp"
const unsigned int Terrain::_size = 800;
const unsigned int Terrain::_nbVertex = 128;

Terrain::Terrain() : Model()//protected
{
    _meshes.push_back(Mesh());
    _dir = "";
}
Terrain::Terrain(unsigned int x, unsigned int z) : Model()// the mesh will not send his variables to openGL
{
    _x = x * _size;
    _z = z * _size;
    _meshes.push_back(_GenerateTerrain(x, z));
}
Terrain::Terrain(unsigned int x, unsigned int z, const std::string &path) : Model()
{
    _x = x * _size;
    _z = z * _size;
    std::cout << "terrain " << std::endl;
    _meshes.push_back(_GenerateTerrain(x, z));
    std::cout << "terrain2 " << std::endl;
    LoadTexture(path);
    std::cout << "terrain3 : " << _meshes.size() << "  " << _meshes[0].faces[10] << std::endl;

   _meshes[0].SendToOpenGL();
}

Terrain::Terrain(Terrain const & src) : Model(src)
{
    *this = src;
}

Terrain::~Terrain(void) {}

Terrain &	Terrain::operator = (Terrain const & rhs)
{
    static_cast<Model>(*this) = static_cast<Model>(rhs);
    _x = rhs._x;
    _z = rhs._z;
    return *this;
}
/*void        Terrain::Draw(const Shader &shader)
{
    Model::Draw(shader);
}*/
Mesh       Terrain::_GenerateTerrain(unsigned int xSize, unsigned int zSize)
{
    //const unsigned int nb_vert = (xSize + 1) * (zSize + 1);
    std::cout << "xSize =  " << xSize << " zSize =  " << zSize<< std::endl;
    std::vector<unsigned int>   faces((xSize * zSize)* 6);
    //std::vector<unsigned int>   faces(nb_vert* 6);
    std::vector<Vertex>     vertices;
    for (unsigned int i = 0,  z = 0; z <= zSize; z++)
    {
        for (unsigned int x = 0; x <= xSize; x++)
        {
            Vertex vert;
            vert.position.x = x;
            vert.position.y = 0;
            vert.position.z = z;
            vert.texCoord.x = static_cast<float>(x);
            vert.texCoord.y = static_cast<float>(z);
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

//Mesh       Terrain::GenerateTerrain(unsigned int x, unsigned int z)
Mesh      _OldGenerateTerrain(unsigned int x, unsigned int z)
{
    unsigned int _size = 0; //suppress
    unsigned int _nbVertex = 0; //suppress
    std::cout << "generate terrain" << std::endl;
    //const unsigned int nb_vert = x * z;
    const unsigned int nb_vert = _nbVertex * _nbVertex;
    //float vertices[count * 3];// = new float[count * 3];
    //float[] normals = new float[count * 3];
    //float textureCoords[count * 2];// = new float[count * 2];
   // int indices[6 * (_nbVertex - 1) * (_nbVertex - 1)]; //= new int[6*(_nbVertex-1)*(_nbVertex-1)];
    //int index = 0;
    //std::vector<Vertex>         vertices;
    if( x== z)
        x = z;
    std::vector<unsigned int>   faces(nb_vert);
    std::vector<Vertex>     verts(nb_vert);
    const float divider = (float)(_nbVertex - 1);
    for (unsigned int i = 0; i < _nbVertex; i++)
    {
        for (unsigned int j = 0; j < _nbVertex; j++)
        {
            Vertex vert;
            vert.position.x = (float)j / divider * _size;
            vert.position.y = 0;
            vert.position.z = (float)i / divider * _size;
            /*vertices[index * 3] = (float)j / divider * _size;
            vertices[index * 3 + 1] = 0;
            vertices[index * 3 + 2] = (float)i / divider * _size;*/
            vert.normal.x = 0;
            vert.normal.y = 1;
            vert.normal.z = 0;
            /*textureCoords[index * 2] = (float)j / divider;
            textureCoords[index * 2 + 1] = (float)i / divider;*/
            vert.texCoord.x = (float)j / divider * _size;
            vert.texCoord.y = (float)i / divider * _size;
            //index++;
            verts.push_back(vert);
        }
    }
    //index = 0;
    for (unsigned int gz = 0; gz < _nbVertex - 1; gz++)
    {
        for (unsigned int gx = 0; gx < _nbVertex - 1; gx++)
        {
            int topLeft = (gz * _nbVertex) + gx;
            int topRight = topLeft + 1;
            int bottomLeft = ((gz + 1) * _nbVertex) + gx;
            int bottomRight = bottomLeft + 1;
            faces.push_back(topLeft);
            faces.push_back(bottomLeft);
            faces.push_back(topRight);
            faces.push_back(topRight);
            faces.push_back(bottomLeft);
            faces.push_back(bottomRight);
            /*indices[index++] = topLeft;
            indices[index++] = bottomLeft;
            indices[index++] = topRight;
            indices[index++] = topRight;
            indices[index++] = bottomLeft;
            indices[index++] = bottomRight;*/
        }
    }
    Mesh mesh = Mesh();
    mesh.vertices = verts;
    mesh.faces = faces;
    return mesh;
		//return loader.loadToVAO(vertices, textureCoords, normals, indices);
}

bool    Terrain::LoadTexture(const std::string & path) 
{
    _dir  = path.substr(0, path.find_last_of('/'));
    _meshes[0].textures.push_back(Model::_LoadSimpleTexture(eTextureType::Specular ,path));
    return true;
}   