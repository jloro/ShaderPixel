
#include "SdlEvent.hpp"
#include <iostream>

int				ft_mouse(int x, int y, t_data *data)
{
}
static int		keypress_do_event(int key, int repeat, t_data *data)
{
	static t_funar_keyb		*tab_fct = NULL;
	static int				size_tab;
	int						i;

	if (tab_fct == NULL)
		tab_fct = fill_funar_keypress(&size_tab);
	i = 0;
	if (key == SDLK_ESCAPE)
	{
		free(tab_fct);
		return (-1);
	}
	while (i < size_tab)
	{
		if (tab_fct[i].key == key)
		{
			if (tab_fct[i].f && (!repeat || tab_fct[i].repeat == repeat))
				tab_fct[i].f(data);
			return (1);
		}
		i++;
	}
	return (0);
}

static int		keyrelease_do_event(int key, t_data *data)
{
	static t_funar_keyb		*tab_fct = NULL;
	static int				size_tab;
	int						i;

	if (tab_fct == NULL)
		tab_fct = fill_funar_keyrelease(&size_tab);
	i = 0;
	if (key == SDLK_ESCAPE)
	{
		free(tab_fct);
		return (-1);
	}
	while (i < size_tab)
	{
		if (tab_fct[i].key == key)
		{
			if (tab_fct[i].f)
				tab_fct[i].f(data);
			return (1);
		}
		i++;
	}
	return (0);
}

int				ft_keyboard(int key, int repeat, SDL_Event *event, t_data *data)
{
	int			ret;

	ret = 0;
	if (event->type == SDL_KEYDOWN)
	{
		if ((ret = keypress_do_event(key, repeat, data)) == 1)
			return (0);
	}
	else if (event->type == SDL_KEYUP)
		if ((ret = keyrelease_do_event(key, data)) == 1)
			return (0);
	if (ret == -1)
	{
		keyrelease_do_event(key, data);
		//ft_putstr("quitting...\n");
		//ft_exit(&data);
	}
	return (0);
}

static int		ft_event(SDL_Event *event, t_data *data)
{
	while (SDL_PollEvent(event))
	{
		if (event->type == SDL_QUIT)
			return (1);
		else if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP)
			ft_keyboard(event->key.keysym.sym, event->key.repeat, event, data);
		/*else if (event->type == SDL_MOUSEMOTION)
			ft_mouse(event->motion.x, event->motion.y, data);
		else if (event->type == SDL_MOUSEBUTTONDOWN &&
				event->button.button == SDL_BUTTON_LEFT)*/
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
}