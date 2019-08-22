/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PostProcess.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jloro <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/08/22 15:22:49 by jloro             #+#    #+#             */
/*   Updated: 2019/08/22 15:43:57 by jloro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POSTPROCESS_HPP
# define POSTPROCESS_HPP

# include "Shader.hpp"
# include "IGameObject.hpp"

class PostProcess : public Shader, public Engine42::IGameObject
{
	public:
		PostProcess(std::vector<const char *> shaderSource, std::vector<GLenum> shaderType);
		virtual~PostProcess();

		virtual void	Update(void);
		virtual void 	FixedUpdate(void);

		virtual void	use(void) const;
	private:
		int	_effect;
		float	_offset;
		glm::mat3 _kernel;
		void _ChangeKernel();
};

#endif
