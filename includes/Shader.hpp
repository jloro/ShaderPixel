/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/18 14:19:23 by jloro             #+#    #+#             */
/*   Updated: 2019/06/18 16:24:20 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADER_HPP
# define SHADER_HPP

# include <vector>
# include "glad.h"
# include <glm.hpp>
# include "Camera.hpp"
# include "SdlWindow.hpp"

class Shader
{
	public:
		Shader(std::vector<const char *> shaderSource, std::vector<GLenum> shaderType);
		virtual~Shader();

		void	use(void) const;
		void	setBool(const std::string &name, bool value) const;
		bool	GetIsRayMarching(void);
		void	SetIsRayMarching(bool value);

		void setInt(const std::string &name, int value) const;
		void setFloat(const std::string &name, float value) const;
		void setVec2(const std::string &name, const glm::vec2 &value) const;
		void setVec2(const std::string &name, float x, float y) const;
		void setVec3(const std::string &name, const glm::vec3 &value) const;
		void setVec3(const std::string &name, float x, float y, float z) const;
		void setVec4(const std::string &name, const glm::vec4 &value) const;
		void setVec4(const std::string &name, float x, float y, float z, float w) const;
		void setMat2(const std::string &name, const glm::mat2 &mat) const;
		void setMat3(const std::string &name, const glm::mat3 &mat) const;
		void setMat4(const std::string &name, const glm::mat4 &mat) const;
		void SetUpUniforms(const Camera &cam, const SdlWindow &win) const ;

	private:
		void	_checkCompileError(GLuint shader);
		bool	_isRayMarching;

		GLuint				_program;
};

#endif
