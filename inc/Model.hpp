/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Model.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/20 12:28:53 by jloro             #+#    #+#             */
/*   Updated: 2019/06/20 15:17:04 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODEL_HPP
# define MODEL_HPP

# include "Mesh.hpp"
# include <assimp/scene.h>

unsigned int TextureFromFile(const char *path, const std::string &directory);
class Model
{
	public:
		Model(const char* path);
		virtual~Model();

		void Draw(Shader shader) const;
	private:
		std::vector<Mesh>	_meshes;
		std::string			_dir;

		void				_LoadModel(std::string path);
		void				_ProcessNode(aiNode *node, const aiScene *scene);
		Mesh				_ProcessMesh(aiMesh *mesh, const aiScene *scene);
		std::vector<Texture>	_LoadMaterialTexture(aiMaterial *mat, aiTextureType type, eType typeName);
};

#endif
