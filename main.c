#include "survive.h"

SDL_Window*		window;
SDL_Renderer*	renderer;
SDL_Texture*	target;
SDL_Cursor*		cursor;
TTF_Font*		font;
bool			fullscreen;
SDL_DisplayMode	desktopmode;
SDL_DisplayMode windowmode;
Uint32 			mousebuttons;
SDL_Point		mousepos;
const Uint8*	keys;


Mix_Chunk* sfx_sword;

/*=========
Cleanup
=========*/
void Cleanup(void)
{
	// free game resources
	ClearMap();
	ClearEntities();

	// free SDL resources
	SDL_FreeCursor(cursor);
	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

/*=========
SetupWindow
=========*/
void SetupWindow(void)
{
	int flags = SDL_Init(SDL_INIT_EVERYTHING);
	if (flags < 0)
	{
		printf("%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	flags = IMG_Init(IMG_INIT_PNG);
	if (!(flags & IMG_INIT_PNG))
	{
		printf("%s\n", IMG_GetError());
		exit(EXIT_FAILURE);
	}
	flags = TTF_Init();
	if (flags < 0)
	{
		printf("%s\n", TTF_GetError());
		exit(EXIT_FAILURE);
	}

	flags = Mix_Init(MIX_INIT_OGG);
	if (flags < 0)
	{
		printf("%s\n", Mix_GetError());
		exit(EXIT_FAILURE);
	}
	flags = Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);
	if (flags < 0)
	{
		printf("%s\n", Mix_GetError());
		exit(EXIT_FAILURE);
	}

	Mix_Music* music = NULL;
	music = Mix_LoadMUS("data/BRPG_Vanquisher_FULL_Loop.wav");
	if (music == NULL)
	{
		printf("%s\n", Mix_GetError());
		exit(EXIT_FAILURE);
	}
	flags = Mix_PlayMusic(music, -1);
	if (flags < 0)
	{
		printf("%s\n", Mix_GetError());
		exit(EXIT_FAILURE);
	}
	Mix_VolumeMusic(64);

	sfx_sword = Mix_LoadWAV("data/whoosh3.ogg");
	if (sfx_sword == NULL)
	{
		printf("%s\n", Mix_GetError());
		exit(EXIT_FAILURE);
	}

	window = SDL_CreateWindow("Survive", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window)
	{
		printf("%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
//	renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (!renderer)
	{
		printf("%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// store off the desktop mode for later use
	flags = SDL_GetDesktopDisplayMode(0, &desktopmode);
	if (flags < 0)
	{
		printf("%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	flags = SDL_GetWindowDisplayMode(window, &windowmode);
	if (flags < 0)
	{
		printf("%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// create a target texture to render everything to, this is then scaled up to the actual full screen resolution when we draw the scene
	// we use this method in combination with SDL_RenderSetLogicalSize, as using SDL_RenderSetLogicalSize alone will scale each draw call individually,
	// leading to seams between tiles, due to the use of integers
	target = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, windowmode.w, windowmode.h);
	if (!target)
	{
		printf("%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	flags = SDL_RenderSetLogicalSize(renderer, windowmode.w, windowmode.h);
	if (flags < 0)
	{
		printf("%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// set crosshair / mouse cursor
	/*SDL_Surface* xhair = IMG_Load("sprites/xhair.png");
	if (!xhair)
	{
		printf("%s\n", IMG_GetError());
		exit(EXIT_FAILURE);
	}
	cursor = SDL_CreateColorCursor(xhair, xhair->w / 2, xhair->h / 2);
	SDL_FreeSurface(xhair);
	if (!cursor)
	{
		printf("%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_SetCursor(cursor);*/


	// load font - we use TEXT_SIZE * 2 because it looks better to create a bigger font then scale it back down when drawing
	font = TTF_OpenFont("data/PressStart2P.ttf", TEXT_SIZE * 2);
	if (!font)
	{
		printf("%s\n", TTF_GetError());
		exit(EXIT_FAILURE);
	}

	// set window icon
	/*	SDL_Surface* icon = IMG_Load("sprites/icon.png");
	if (!icon)
	{
		printf("%s\n", IMG_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_SetWindowIcon(window, icon);
	SDL_FreeSurface(icon);	*/

	keys = SDL_GetKeyboardState(NULL);					// keys is a pointer to an internal SDL array with the current keys held 
	SDL_SetWindowGrab(window, SDL_TRUE);				// capture mouse

	SDL_ShowCursor(SDL_DISABLE);	// disable mouse

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");	// chunky pixels

	printf("Created Window: %i %i\n", windowmode.w, windowmode.h);
}

/*==========
SeedRNG
==========*/
void SeedRNG(void)
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	srand(SDL_GetTicks() + x + y);
}

/*=========
main
=========*/
#pragma warning( push )  
#pragma warning( disable : 4100 )
int main(int argc, char* argv[])
#pragma warning ( pop )
{
	printf("Survive - compiled on %s %s\n", __DATE__, __TIME__);
	SetupWindow();
	//SetupMenu();
	SeedRNG();
	CreateNewMap();
	SpawnEntities();
	GameLoop();
	Cleanup();
	return EXIT_SUCCESS;
}

