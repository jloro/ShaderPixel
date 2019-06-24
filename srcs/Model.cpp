/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/20 12:44:53 by jloro             #+#    #+#             */
/*   Updated: 2019/06/24 11:33:43 by jules            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Model.hpp"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Model::Model(void)
{
}

Model::Model(const char* path, glm::mat4 model) : _model(model)
{
	_LoadModel(path);
}
Model::Model(const Model &src)
{
    *this = src;
}
void Draw(void) 
{
	std::cout << "Draw Method not implemented yet" << std::endl;
	//_shader.setMat4("model", _model)
	// for (unsigned int i = 0; i < _meshes.size(); i++)
	// 	_meshes[i].Draw(_shader);
}

Model::~Model() {}

void	Model::Draw(Shader shader) const
{
	shader.setMat4("model", _model);
	for (unsigned int i = 0; i < _meshes.size(); i++)
		_meshes[i].Draw(shader);
}
Model & Model::operator=(const Model &rhs)
{
    this->_dir = rhs._dir;
    this->_meshes = rhs._meshes;
    return *this;
}

void	Model::_LoadModel(std::string path)
{
	Assimp::Importer	import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
	_dir = path.substr(0, path.find_last_of('/'));

	_ProcessNode(scene->mRootNode, scene);
}

void	Model::_ProcessNode(aiNode *node, const aiScene *scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
		_meshes.push_back(_ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene));
	for (unsigned int i = 0; i < node->mNumChildren; i++)
		_ProcessNode(node->mChildren[i], scene);
}

Mesh	Model::_ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
	std::vector<Vertex>	vertices;
	std::vector<unsigned int>	faces;
	std::vector<Texture>	textures;

	std::cout <<  "Load mesh" << std::endl;
	//Get vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		if (mesh->HasNormals())
			vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		else
			vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
		if (mesh->HasTextureCoords(0))
			vertex.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		else
			vertex.texCoord = glm::vec2(0.0f, 0.0f);
		vertices.push_back(vertex);
	}
	//Get faces
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace	face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			faces.push_back(face.mIndices[j]);
	}
	aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseMaps = _LoadMaterialTexture(material, aiTextureType_DIFFUSE, Diffuse);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	std::vector<Texture> specularMaps = _LoadMaterialTexture(material, aiTextureType_SPECULAR, Specular);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	return Mesh(vertices, faces, textures);
}

std::vector<Texture>	Model::_LoadMaterialTexture(aiMaterial *mat, aiTextureType type, eTextureType typeName)
{
	std::vector<Texture>	textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		texture.id = TextureFromFile(str.C_Str(), _dir);
		texture.type = typeName;
		textures.push_back(texture);
	}
	return textures;
}

unsigned int TextureFromFile(const char *path, const std::string &directory)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void	Model::SetModel(glm::mat4 model) { _model = model; }
glm::mat4	Model::GetModel(void) const { return _model; }