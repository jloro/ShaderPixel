/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/20 11:31:23 by jloro             #+#    #+#             */
/*   Updated: 2019/06/20 16:28:46 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> faces, std::vector<Texture> textures) : _vertices(vertices), _faces(faces), _textures(textures)
{
	_SendToOpenGL();
}

Mesh::~Mesh() {}

void	Mesh::_SendToOpenGL()
{
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);
	glGenVertexArrays(1, &_vao);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _faces.size(), &_faces[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

	glBindVertexArray(0);
}

void	Mesh::Draw(Shader shader) const
{
	unsigned int	diffuseNb = 0;
	unsigned int	specularNb = 0;
	std::string		name;
	std::string		number;

	for (unsigned int i = 0; i < _textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		if (_textures[i].type == Diffuse)
		{
			number = std::to_string(diffuseNb++);
			name = "texture_diffuse";
		}
		else if (_textures[i].type == Specular)
		{
			number = std::to_string(specularNb++);
			name = "texture_specular";
		}
		shader.setInt((name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, _textures[i].id);
	}

	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _faces.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}
