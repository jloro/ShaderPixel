/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Framebuffer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jules <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/14 14:59:11 by jules             #+#    #+#             */
/*   Updated: 2019/08/14 16:03:12 by jules            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRAMEBUFFER_HPP
# define FRAMEBUFFER_HPP

# include "Shader.hpp"
# include "Model.hpp"

class Framebuffer
{
	public:
		Framebuffer(int width, int height, Shader* shader, Model* model);
		void genTexture() const;
		void Draw() const;
		virtual	~Framebuffer();

		unsigned int	GetTexture(void) const;

	private:
		unsigned int	_fbo;
		unsigned int	_colorBuffer;
		unsigned int	_rbo;
		unsigned int	_quadVAO;
		unsigned int	_quadVBO;
		Shader*			_shader;
		Shader*			_shaderModel;
		Model*			_model;
};

#endif
