#ifndef IGAMEOBJECT_HPP
# define IGAMEOBJECT_HPP
# include "SDL.h"

namespace Engine42
{
	class IGameObject
	{
		public:
			// IGameObject(void){}
			// IGameObject(const IGameObject & src);
			virtual ~IGameObject(void) {}
			// IGameObject &	operator=(const IGameObject & rhs);

			virtual void	Update(void) = 0;
			virtual void	FixedUpdate(void) = 0;
			// virtual int		Collide(IGameObject *other) = 0;
		protected:
	};
}
#endif
