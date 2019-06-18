
#include "SdlWindow.hpp"

int				main(int ac, char **av)
{
	if (ac < -1 && av == nullptr)
		return 1;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Erreur lors de l'initialisation de la SDL : " << std::endl;
		std::cout << SDL_GetError() << std::endl;;
		SDL_Quit();
		return (EXIT_SUCCESS);
	}
	SDL_Init(SDL_INIT_VIDEO);
	SdlWindow	win(800, 400, false, true, "test");
	win.CreateGlContext(4, 1, true, 24);
	std::string pouet;
	std::cin >> pouet;
}
