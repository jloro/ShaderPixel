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
		Model(const char* path, glm::mat4 model);
		Model(const Model & rhs);
		virtual~Model();
/*  public functions    */
		void Draw(Shader shader) const;
		void Draw(void) const;
        Model & operator=(const Model &rhs);
		glm::mat4	GetModel(void) const;
		void		SetModel(glm::mat4 model);
	private:
/*  private variables    */
		std::vector<Mesh>	_meshes;
		std::string			_dir;
		glm::mat4			_model;
		//Shader				&_shader;
/*  private functions    */
		void					_LoadModel(std::string path);
		void					_ProcessNode(aiNode *node, const aiScene *scene);
		Mesh					_ProcessMesh(aiMesh *mesh, const aiScene *scene);
		std::vector<Texture>	_LoadMaterialTexture(aiMaterial *mat, aiTextureType type, eTextureType typeName);
};

#endif