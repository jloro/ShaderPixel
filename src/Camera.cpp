/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/18 16:50:38 by jloro             #+#    #+#             */
/*   Updated: 2019/06/18 16:57:25 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Camera.hpp"
#include <gtc/matrix_transform.hpp>

Camera::Camera(void) : _view(glm::mat4(1.0f)), _projection(glm::mat4(1.0f))
{
	_view = glm::translate(_view, glm::vec3(0.0f, 0.0f, -3.0f));
	_projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

glm::mat4	Camera::GetMatView(void) const { return _view; }
glm::mat4	Camera::GetMatProj(void) const { return _projection; }
