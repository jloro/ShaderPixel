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
/*  constructors / destructors  */
		Mesh(void); 
		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> faces, std::vector<Texture> textures);
		Mesh(Mesh const & src); 
		virtual~Mesh();
/*  public variables  */
		std::vector<Vertex>			vertices;
		std::vector<unsigned int>	faces;
		std::vector<Texture>		textures;
/*  public functions  */
		Mesh &		operator=(Mesh const & rhs);
		void		Draw(const std::shared_ptr<Shader>  shader) const;
		void		SendToOpenGL(void);
	private:
/*  private variables  */
		GLuint						_vao;
		GLuint						_vbo;
		GLuint						_ebo;
/*  private functions */
};

#endif
