/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/20 11:23:36 by jloro             #+#    #+#             */
/*   Updated: 2019/06/20 16:44:28 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESH_HPP
# define MESH_HPP

# include <glm.hpp>
# include <string>
# include <vector>
# include "glad.h"
# include "Shader.hpp"
# include "Vertex.hpp"
# include "Texture.hpp"

class Mesh
{
	public:
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> faces, std::vector<Texture> textures);
		virtual~Mesh();

		void	Draw(Shader shader) const;
	private:
		std::vector<Vertex>			_vertices;
		std::vector<unsigned int>	_faces;
		std::vector<Texture>		_textures;

		GLuint						_vao;
		GLuint						_vbo;
		GLuint						_ebo;

		void						_sendToOpenGL(void);
};

#endif
