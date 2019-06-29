/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/18 14:24:02 by jloro             #+#    #+#             */
/*   Updated: 2019/06/18 16:22:46 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Shader.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <glm.hpp>

Shader::Shader(std::vector<const char *> shaderSource, std::vector<GLenum> shaderType)
{
	std::ifstream	ifs;
	std::string*	tmpSource;
	GLuint			tmpShader;
	const char *	tmp;

	_program = glCreateProgram();
	_isRayMarching = false;
	for (unsigned int i = 0; i < shaderSource.size(); i++)
	{
		ifs.open(shaderSource[i], std::ifstream::in);
		tmpSource = new std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
		tmpShader = glCreateShader(shaderType[i]);
		tmp = tmpSource->c_str();
		glShaderSource(tmpShader, 1, &tmp, NULL);
		glCompileShader(tmpShader);
		_checkCompileError(tmpShader);
		glAttachShader(_program, tmpShader);
		glDeleteShader(tmpShader);
		ifs.close();
		delete tmpSource;
	}
	glLinkProgram(_program);
}

Shader::~Shader(void) {}

void	Shader::_checkCompileError(GLuint shader)
{
	int		success;
	char	infoLog[512];

	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}
std::ostream &operator<<(std::ostream &o, const glm::vec3 & vec)
{
	o << "x: " << vec.x << ", y: " << vec.y << ", z: " << vec.z;
	return o;
}
std::ostream &operator<<(std::ostream &o, const glm::vec2 & vec)
{
	o << "x: " << vec.x << ", y: " << vec.y;
	return o;
}
void 	Shader::SetUpUniforms(const Camera &cam, const SdlWindow &win) const
{
	/*std::cout << "camPos :" <<  cam._pos << std::endl;
	std::cout << "camDir "<< cam._dir<< std::endl;
	std::cout << "camUp " << cam._up<< std::endl;
	std::cout << "camPitch " << cam._pitch<< std::endl;
	std::cout << "Cam Yaw " << cam._yaw  << std::endl;
	std::cout << "width " << win.GetWidth() << std::endl;
	std::cout << "heihgt " << win.GetHeight() << std::endl;*/
	setMat4("view", cam.GetMatView());
	setMat4("projection", cam.GetMatProj());
	setVec3("uCamPos", cam._pos);
	setVec3("uDir", cam._dir);
	setVec3("uUp", cam._up);
	setVec2("uResolution", glm::vec2(win.GetWidth(), win.GetHeight()));
	setVec2("uRotation", glm::vec2(cam._pitch, cam._yaw));
	setFloat("uFov", glm::radians(45.0f));
}
void	Shader::use(void) const
{
	glUseProgram(_program);
}
bool	Shader::GetIsRayMarching(void) { return _isRayMarching; }
void	Shader::SetIsRayMarching(bool value) { _isRayMarching = value;}

// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) const
{         
	glUniform1i(glGetUniformLocation(_program, name.c_str()), (int)value); 
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const
{ 
	glUniform1i(glGetUniformLocation(_program, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const
{ 
	glUniform1f(glGetUniformLocation(_program, name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{ 
	glUniform2fv(glGetUniformLocation(_program, name.c_str()), 1, &value[0]); 
}
void Shader::setVec2(const std::string &name, float x, float y) const
{ 
	glUniform2f(glGetUniformLocation(_program, name.c_str()), x, y); 
}
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{ 
	glUniform3fv(glGetUniformLocation(_program, name.c_str()), 1, &value[0]); 
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{ 
	glUniform3f(glGetUniformLocation(_program, name.c_str()), x, y, z); 
}
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{ 
	glUniform4fv(glGetUniformLocation(_program, name.c_str()), 1, &value[0]); 
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{ 
	glUniform4f(glGetUniformLocation(_program, name.c_str()), x, y, z, w); 
}
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
