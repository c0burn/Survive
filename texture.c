#include "survive.h"

typedef struct Texture
{
	char name[256];
	SDL_Texture* texture;
} Texture;

/*==========
CreateTextureFromText

create an SDL_Texture containing text rendered with SDL_TTF
returns a pointer to the SDL_Texture created, or NULL on failure

you can pass a colour and font style to be used
==========*/
SDL_Texture* CreateTextureFromText(const char* str, SDL_Color colour, int style)
{
	TTF_SetFontStyle(font, style);

	SDL_Surface* tempsurf = TTF_RenderText_Blended(font, str, colour);
	if (tempsurf == NULL)
	{
		printf("%s\n", TTF_GetError());
		return NULL;
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, tempsurf);
	SDL_FreeSurface(tempsurf);
	if (tex == NULL)
	{
		printf("%s\n", SDL_GetError());
		return NULL;
	}
	return tex;
}

/*==========
CropSurface

takes a pointer to an SDL_Surface and crops it according to the dimensions of an SDL_Rect
return value is a pointer so a new SDL_Surface or NULL on failure
this function is only to be used internally by CreateCroppedTexture
==========*/
static SDL_Surface* CropSurface(SDL_Surface* source, SDL_Rect rect)
{
	if (source == NULL)
	{
		printf("CropSurface: source is NULL\n");
		return NULL;
	}
	if (SDL_RectEmpty(&rect))
	{
		printf("CropSurface: rect is EMPTY\n");
		return NULL;
	}

	SDL_Surface* cropsurf = SDL_CreateRGBSurface(source->flags, rect.w, rect.h, source->format->BitsPerPixel,
		source->format->Rmask, source->format->Gmask, source->format->Bmask, source->format->Amask);
	if (cropsurf == NULL)
	{
		printf("CropSurface: %s\n", SDL_GetError());
		return NULL;
	}

	int err = SDL_BlitSurface(source, &rect, cropsurf, NULL);
	if (err < 0)
	{
		printf("CropSurface: %s\n", SDL_GetError());
		SDL_FreeSurface(cropsurf);
		return NULL;
	}

	return cropsurf;
}

/*=========
CreateCroppedTexture

takes a path to an image and returns a pointer to a new SDL_Texture
which is cropped according to "rect"
=========*/
SDL_Texture* CreateCroppedTexture(const char* path, SDL_Rect rect)
{
	SDL_Surface* surf = IMG_Load(path);
	if (surf == NULL)
	{
		printf("CreateCroppedTexture: %s\n", IMG_GetError());
		return NULL;
	}

	SDL_Surface* cropsurf = CropSurface(surf, rect);
	SDL_FreeSurface(surf);
	if (cropsurf == NULL)
	{
		printf("CreateCroppedTexture: cropsurf is NULL\n");
		return NULL;
	}

	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, cropsurf);
	SDL_FreeSurface(cropsurf);
	if (tex == NULL)
	{
		printf("CreateCroppedTexture: %s\n", SDL_GetError());
		return NULL;
	}

	return tex;
}

/*=========
CreateTexture

takes a path to an image file and returns a pointer to a new SDL_Texture
if shadow = true, replace all non transparent pixels in the image with dark blue (looks nicer than black)
this should not be called directly hence the use of a static function - use LoadTexture
=========*/
static SDL_Texture* CreateTexture(const char* path)
{
	SDL_Surface* surf = IMG_Load(path);
	if (surf == NULL)
	{
		printf("CreateTexture: %s\n", IMG_GetError());
		return NULL;
	}

	/*
	if (shadow)
	{
		Uint32* pixels = (Uint32*)surf->pixels;

		for (int x = 0; x < surf->w; x++)
		{
			for (int y = 0; y < surf->h; y++)
			{
				Uint32 p = pixels[(y * surf->w) + x];
				Uint8 r, g, b, a;
				SDL_GetRGBA(p, surf->format, &r, &g, &b, &a);

				if (a == 255) // this pixel is not transparent
					pixels[(y * surf->w) + x] = SDL_MapRGB(surf->format, 0, 0, 32);
			}
		}
	}
	*/

	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	if (tex == NULL)
	{
		printf("CreateTexture: %s\n", SDL_GetError());
		return NULL;
	}
	return tex;
}

/*==========
LoadTexture
==========*/
SDL_Texture* LoadTexture(const char* path)
{
	static Texture* textures;
	static int numtextures;

	// just return a pointer if the texture is already loaded
	for (int i = 0; i < numtextures; i++)
	{
		if (strcmp(textures[i].name, path) == 0)
			return textures[i].texture;
	}

	SDL_Texture* new = CreateTexture(path);
	if (new == NULL)
	{
		printf("LoadTexture: CreateTetxure returned NULL\n");
		return NULL;
	}

	Texture* temp = realloc(textures, ++numtextures * sizeof(Texture));
	if (temp == NULL)
	{
		perror("LoadTexture: realloc failed");
		exit(EXIT_FAILURE);
	}
	textures = temp;

	snprintf(textures[numtextures - 1].name, sizeof(textures[numtextures - 1].name), "%s", path);
	textures[numtextures - 1].texture = new;
	printf("LoadTexture: created new texture: %i - %s\n", numtextures, path);
	return new;
}
