
#include "SdlWindow.hpp"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags

static int		ft_event(SDL_Event *event, t_data *data)
{
	while (SDL_PollEvent(event))
	{
		if (event->type == SDL_QUIT)
			return (1);
		else if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP)
			ft_keyboard(event->key.keysym.sym, event->key.repeat, event, data);
		else if (event->type == SDL_MOUSEMOTION)
			ft_mouse(event->motion.x, event->motion.y, data);
		else if (event->type == SDL_MOUSEBUTTONDOWN &&
				event->button.button == SDL_BUTTON_LEFT)
			play_shot_sound(data);
		else if (event->type == SDL_WINDOWEVENT && event->window.event
				== SDL_WINDOWEVENT_CLOSE)
		{
			event->type = SDL_KEYDOWN;
			event->key.keysym.sym = SDLK_ESCAPE;
			ft_keyboard(event->key.keysym.sym, event->key.repeat, event, data);
		}
	}
	return (0);
}
void			game_loop(SdlWindow &win)
{
	SDL_Event			event;
	int					quit;
	const unsigned int	fixdelta = 20;
	unsigned int		last_time;
	unsigned int		delta;

	quit = 0;
	last_time = SDL_GetTicks();
	delta = 0.0;
	while (!quit)
	{
		quit = ft_event(&event, data);
		delta += SDL_GetTicks() - last_time;
		if (delta >= fixdelta)
		{
			delta = 0.0;
			rendering(data);
		}
		last_time = SDL_GetTicks();
	}
	ft_exit(&data);
}

int				main(int ac, char **av)
{
	if (ac < -1 && av == nullptr)
		return 1;
	Assimp::Importer importer;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Erreur lors de l'initialisation de la SDL : " << std::endl;
		std::cout << SDL_GetError() << std::endl;;
		SDL_Quit();
		return (EXIT_SUCCESS);
	}
	SdlWindow	win(800, 400, false, true, "test");
	win.CreateGlContext(4, 1, true, 24);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	win.Swap();
	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 trans = glm::mat4(1.0f);
	trans += vec;
	std::string pouet;
	std::cin >> pouet;
}
