/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/18 16:47:43 by jloro             #+#    #+#             */
/*   Updated: 2019/06/18 17:45:18 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_HPP
# define CAMERA_HPP

# include <glm.hpp>

class Camera
{
	public:
		Camera(void);

		glm::mat4	GetMatView(void) const;
		glm::mat4	GetMatProj(void) const;
	private:
		glm::vec3	_pos;
		glm::vec3	_right;
		glm::vec3	_up;
		glm::vec3	_dir;
		glm::mat4	_view;
		glm::mat4	_projection;
};

#endif
