/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/18 16:50:38 by jloro             #+#    #+#             */
/*   Updated: 2019/07/29 12:07:39 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Camera.hpp"
#include "Engine.hpp"
#include <gtc/matrix_transform.hpp>

Camera		*Camera::instance = nullptr;

Camera::Camera(float width, float height) : _moveSpeed(MOVE_SPEED), _mouseSensitivity(MOUSE_SENSITIVITY), _pitch(0.0f), _yaw(-90.0f), _width(width), _height(height)
{
	if (Camera::instance == nullptr)
		instance = this;
	_pos = glm::vec3(0.0f, 0.0f, 8.0f);
	_dir = glm::vec3(0.0f, 0.0f, -1.0f);
	_CalcMatrix();
}

glm::mat4	Camera::GetMatView(void) const { return _view; }
glm::mat4	Camera::GetMatProj(void) const { return _projection; }
glm::vec3	Camera::GetPos(void) const { return _pos; }
glm::vec3	Camera::GetDir(void) const { return _dir; }
glm::vec3	Camera::GetUp(void) const { return _up; }
float		Camera::GetMoveSpeed(void) const { return _moveSpeed; }
float		Camera::GetXRotation(void) const { return _yaw; }
float		Camera::GetYRotation(void) const { return _pitch; }

void 	Camera::Update()
{
	//const SDL_Event	&event = Engine42::Engine::GetEvent();
	
	/* if (event.type == SDL_MOUSEMOTION)
		LookAround(event.motion.xrel, -event.motion.yrel);*/
	//else //if (event.type == SDL_KEYDOWN)
	//{
		const Uint8 	*keys = Engine42::Engine::GetKeyInput();
		if (keys[SDL_SCANCODE_W])
			Move(eCameraDirection::Forward, Engine42::Time::GetDeltaTime());
		if (keys[SDL_SCANCODE_S])
			Move(eCameraDirection::Backward, Engine42::Time::GetDeltaTime());
		if (keys[SDL_SCANCODE_D])
			Move(eCameraDirection::Right, Engine42::Time::GetDeltaTime());
		if (keys[SDL_SCANCODE_A])
			Move(eCameraDirection::Left, Engine42::Time::GetDeltaTime());
		if (keys[SDL_SCANCODE_LCTRL])
			Move(eCameraDirection::Down, Engine42::Time::GetDeltaTime());
		if (keys[SDL_SCANCODE_SPACE])
			Move(eCameraDirection::Up, Engine42::Time::GetDeltaTime());
		if (keys[SDL_SCANCODE_R])
			Engine42::Engine::ReloadShaders();
		if (keys[SDL_SCANCODE_LSHIFT])
			_sprint = true;
		else
			_sprint = false;

	//}
}
void	Camera::FixedUpdate() {}

void	Camera::Move(eCameraDirection dir, float deltaTime)
{
	float moveSpeed = _sprint ? _moveSpeed * 4 : _moveSpeed;
	if (dir == Forward)
		_pos += moveSpeed * deltaTime * _dir;
	else if (dir == Backward)
		_pos -= moveSpeed * deltaTime * _dir;
	else if (dir == Right)
		_pos += moveSpeed * deltaTime * _right;
	else if (dir == Left)
		_pos -= moveSpeed * deltaTime * _right;
	else if (dir == Up)
		_pos += moveSpeed * deltaTime * _up;
	else if (dir == Down)
		_pos -= moveSpeed * deltaTime * _up;

	_CalcMatrix();
}

void	Camera::_CalcMatrix()
{
	_right = glm::normalize(glm::cross(_dir, glm::vec3(0.0f, 1.0f, 0.0f)));
	_up = glm::normalize(glm::cross(_right, _dir));
	_view = glm::lookAt(_pos, _pos + _dir, _up);
	_projection = glm::perspective(glm::radians(FOV), _width / _height, 0.1f, 100.0f);
}

void	Camera::LookAround(float xoffset, float yoffset)
{

	xoffset *= _mouseSensitivity;
	yoffset *= _mouseSensitivity;

	_yaw   += xoffset;
	_pitch = glm::clamp(_pitch + yoffset, -89.0f, 89.0f);

	_dir.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_dir.y = sin(glm::radians(_pitch));
	_dir.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	_dir = glm::normalize(_dir);
	_CalcMatrix();
}

glm::mat4	Camera::GetViewMatrix(void) const
{
	return _view;
}
