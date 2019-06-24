/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vertex.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/20 16:41:44 by jloro             #+#    #+#             */
/*   Updated: 2019/06/20 16:42:09 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VERTEX_HPP
# define VERTEX_HPP

# include <glm.hpp>

struct Vertex {
	glm::vec3	position;
	glm::vec3	normal;
	glm::vec2	texCoord;
};

#endif