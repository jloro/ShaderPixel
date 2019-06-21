/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/18 16:50:38 by jloro             #+#    #+#             */
/*   Updated: 2019/06/21 12:18:14 by jules            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Camera.hpp"
#include <gtc/matrix_transform.hpp>

Camera		*Camera::instance = nullptr;

Camera::Camera(float width, float height, float posX, float posY) : _deltaTime(0.0f), _lastFrame(0.0f), _moveSpeed(MOVE_SPEED),
			_mouseSensitivity(MOUSE_SENSITIVITY), _lastPosX(posX), _lastPosY(posY), _pitch(0.0f), _yaw(-90.0f), _width(width), _height(height)
{
	if (Camera::instance == nullptr)
		instance = this;
	_pos = glm::vec3(0.0f, 0.0f, 3.0f);
	_up = glm::vec3(0.0f, 1.0f, 0.0f);
	_dir = glm::vec3(0.0f, 0.0f, -1.0f);
	_right = glm::vec3(1.0f, 0.0f, 0.0f);
	_CalcMatrix();
}

glm::mat4	Camera::GetMatView(void) const { return _view; }
glm::mat4	Camera::GetMatProj(void) const { return _projection; }

void	Camera::Move(eCameraDirection dir)
{
	if (dir == Forward)
		_pos += _moveSpeed * _deltaTime * _dir;
	if (dir == Backward)
		_pos -= _moveSpeed * _deltaTime * _dir;
	if (dir == Right)
		_pos += _moveSpeed * _deltaTime * _right;
	if (dir == Left)
		_pos -= _moveSpeed * _deltaTime * _right;
	if (dir == Up)
		_pos += _moveSpeed * _deltaTime * _up;
	if (dir == Down)
		_pos -= _moveSpeed * _deltaTime * _up;

	_CalcMatrix();
}

void	Camera::_CalcMatrix()
{
	_view = glm::lookAt(_pos, _pos + _dir, _up);
	_projection = glm::perspective(glm::radians(FOV), _width / _height, 0.1f, 100.0f);
}

void	Camera::UpdateFrame()
{
	//float currentFrame = glfwGetTime();
	//_deltaTime = currentFrame - _lastFrame;
	//_lastFrame = currentFrame;
}

void	Camera::LookAround(float posX, float posY)
{
	float xoffset = posX - _lastPosX;
	float yoffset = _lastPosY - posY; 
	_lastPosX = posX;
	_lastPosY = posY;

	xoffset *= _mouseSensitivity;
	yoffset *= _mouseSensitivity;

	_yaw   += xoffset;
	_pitch += yoffset;

	if(_pitch > 89.0f)
		_pitch = 89.0f;
	if(_pitch < -89.0f)
		_pitch = -89.0f;

	_dir.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_dir.y = sin(glm::radians(_pitch));
	_dir.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_dir = glm::normalize(_dir);
	_CalcMatrix();
}


