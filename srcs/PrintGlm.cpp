/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PrintGlm.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/27 19:15:52 by jloro             #+#    #+#             */
/*   Updated: 2019/07/27 19:17:46 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PrintGlm.hpp"

std::ostream &operator<<(std::ostream &o, glm::vec3 & vec)
{
	o << "x: " << vec.x << ", y: " << vec.y << ", z: " << vec.z;
	return o;
}
std::ostream &operator<<(std::ostream &o, glm::mat3 & mat)
{
	for (int i =0; i < 3; ++i)
	{
		o << mat[i] << std::endl;
		//o << "x: " << vec.x << ", y: " << vec.y << ", z: " << vec.z;
	}
	return o;
}
