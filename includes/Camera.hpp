/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/18 16:47:43 by jloro             #+#    #+#             */
/*   Updated: 2019/06/21 14:12:02 by jules            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_HPP
# define CAMERA_HPP

# include <glm.hpp>

# define MOVE_SPEED 2.5f
# define MOUSE_SENSITIVITY 0.1f
# define FOV 45.0f

enum eCameraDirection { Forward, Backward, Right, Left, Up, Down};

class Camera
{
	public:
		Camera(float width, float height, float posX, float posY);

		glm::mat4	GetMatView(void) const;
		glm::mat4	GetMatProj(void) const;

		void	Move(eCameraDirection dir, float deltaTime);
		void	LookAround(float posX, float posY);
		void	UpdateFrame();
		void	ResizeWindow(float newWidth, float newHeight);

		static Camera	*instance;
	private:
		void	_CalcMatrix();

		glm::vec3	_pos;
		glm::vec3	_right;
		glm::vec3	_up;
		glm::vec3	_dir;

		glm::mat4	_view;
		glm::mat4	_projection;

		float		_moveSpeed;
		float		_mouseSensitivity;

		float		_lastPosX;
		float		_lastPosY;
		float		_pitch;
		float		_yaw;

		//Size of window
		float		_width;
		float		_height;
};

#endif
