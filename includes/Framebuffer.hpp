/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Framebuffer.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jules <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/14 14:59:11 by jules             #+#    #+#             */
/*   Updated: 2019/08/15 11:49:33 by jules            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRAMEBUFFER_HPP
# define FRAMEBUFFER_HPP

# include "Shader.hpp"
# include "Model.hpp"
# include "MeshRenderer.hpp"

class Framebuffer : public MeshRenderer
{
	public:
		Framebuffer(int width, int height, std::shared_ptr<Shader> shader, std::shared_ptr<Model> model, Transform trans);
		void genTexture() const;
		virtual void Draw() const;
		virtual	~Framebuffer();

		unsigned int	GetTexture(void) const;

	private:
		unsigned int	_fbo;
		unsigned int	_colorBuffer;
		unsigned int	_rbo;
		unsigned int	_quadVAO;
		unsigned int	_quadVBO;
		std::shared_ptr<Shader>			_shaderModel;
};

#endif
