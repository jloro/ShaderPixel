/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/20 12:28:53 by jloro             #+#    #+#             */
/*   Updated: 2019/06/22 17:03:45 by jules            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODEL_HPP
# define MODEL_HPP

# include "Mesh.hpp"
# include <assimp/scene.h>
# include <glm.hpp>

unsigned int TextureFromFile(const char *path, const std::string &directory);
class Model
{
	public:
/*  constructors / Destructor*/
		Model(void);
		Model(const char* path);
		Model(const Model & rhs);
		virtual~Model();
/*  public functions    */
		virtual void Draw(const std::shared_ptr<Shader>  shader) const;
        Model & operator=(const Model &rhs);
	protected:
/*  protected variables    */
		std::vector<Mesh>	_meshes;
		std::string			_dir;
/*  protected functions    */
		void					_LoadModel(std::string path);
		void					_ProcessNode(aiNode *node, const aiScene *scene);
		Mesh					_ProcessMesh(aiMesh *mesh, const aiScene *scene);
		std::vector<Texture>	_LoadMaterialTexture(aiMaterial *mat, aiTextureType type, eTextureType typeName);
		static Texture			_LoadSimpleTexture(eTextureType typeName, const std::string filename);
		static unsigned int 	_TextureFromFile(const char *path, const std::string &directory);
		static unsigned int 	_TextureFromFile(const std::string &filename) ;
		static std::string 		_GetFilename(const char *path, const std::string &directory);
};

#endif