#include "survive.h"

SDL_Texture** menutexture;
int savedmodecount;

/*==========
RenderMenu
==========*/
void RenderMenu(void)
{
	SDL_Rect rect;
	rect.y = 128;
	
	for (int i = 0; i < 5 + savedmodecount; i++)
	{
		if (i == 0)
			SDL_SetTextureColorMod(menutexture[i], 192, 16, 16);

		SDL_QueryTexture(menutexture[i], NULL, NULL, &rect.w, &rect.h);
		rect.x = (windowmode.w / 2) - (rect.w / 2);
		SDL_RenderCopy(renderer, menutexture[i], NULL, &rect);
		rect.y += 32;
	}
}

int gcd(int a, int b)
{
	int temp;
	while (b != 0)
	{
		temp = a % b;
		a = b;
		b = temp;
	}
	return a;
}

/*==========
SetupMenu
==========*/
void SetupMenu(void)
{
	menutexture = realloc(menutexture, 5 * sizeof(SDL_Texture*));
	if (menutexture == NULL)
	{
		perror("SetupMenu: realloc failed");
		exit(EXIT_FAILURE);
	}

	menutexture[0] = CreateTextureFromText("S U R V I V E", WHITE, TTF_STYLE_BOLD | TTF_STYLE_ITALIC);
	menutexture[1] = CreateTextureFromText("New Game", WHITE, TTF_STYLE_NORMAL);
	menutexture[2] = CreateTextureFromText("High Scores", WHITE, TTF_STYLE_NORMAL);
	menutexture[3] = CreateTextureFromText("Options", WHITE, TTF_STYLE_NORMAL);
	menutexture[4] = CreateTextureFromText("Quit", WHITE, TTF_STYLE_NORMAL);
	
	int monitorcount = SDL_GetNumVideoDisplays();
	printf("Monitor count: %i\n", monitorcount);

	// list available display modes
	int modecount = SDL_GetNumDisplayModes(0);
	printf("SDL_GetNumDisplayModes: %i\n", modecount);
	SDL_DisplayMode mode;
	SDL_DisplayMode* modes = malloc(1 * sizeof(SDL_DisplayMode));
	if (modes == NULL)
	{
		perror("SetupMenu: malloc failed");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < modecount; ++i)
	{
		SDL_GetDisplayMode(0, i, &mode);

		// only store off modes that are smaller than desktop and the same bpp/refresh rate
		if ((mode.refresh_rate == desktopmode.refresh_rate) && (SDL_BITSPERPIXEL(mode.format) == SDL_BITSPERPIXEL(desktopmode.format)))
		{
			if (mode.w != desktopmode.w && mode.h != desktopmode.h)
			{
				savedmodecount++;
				modes = realloc(modes, savedmodecount * sizeof(SDL_DisplayMode));
				modes[savedmodecount - 1] = mode;
			}
		}
	}
	printf("Saved Modes: %i\n", savedmodecount);

	menutexture = realloc(menutexture, (5 + savedmodecount) * sizeof(SDL_Texture*));
	for (int i = 0; i < savedmodecount; ++i)
	{
		char s[64];
		mode = modes[i];
		printf("Mode %i\tbpp %i\t%s\t%i x %i\t%i hz\t%i:%i aspect\n", i, SDL_BITSPERPIXEL(mode.format), SDL_GetPixelFormatName(mode.format), mode.w, mode.h, mode.refresh_rate, mode.w / gcd(mode.w, mode.h), mode.h / gcd(mode.w, mode.h));
		sprintf_s(s, 64, "%u x %u", mode.w, mode.h);
		menutexture[5 + i] = CreateTextureFromText(s, WHITE, TTF_STYLE_NORMAL);
	}

	free(modes);
	
}
