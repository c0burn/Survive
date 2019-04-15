#include "survive.h"

/*==========
RenderHud
==========*/
void RenderHud(void)
{
	static SDL_Texture* health;
	static SDL_Texture* heart;
	if (!health)
		health = CreateTextureFromText("100", RED, TTF_STYLE_BOLD);
	if (!heart)
		heart = LoadTexture("sprites/health.png");

	SDL_Rect rect = (SDL_Rect) { 16, windowmode.h - 48, 32, 32 };
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 30, 128);
	SDL_RenderFillRect(renderer, &rect);
	SDL_RenderCopy(renderer, heart, NULL, &rect);

	SDL_QueryTexture(health, NULL, NULL, &rect.w, &rect.h);
	rect.w = TEXT_SIZE * 4 * 3;
	rect.h = TEXT_SIZE * 4;
	rect.x = 48;
	rect.y = windowmode.h - 16 - rect.h;

	// draw a background for easier visibility of text
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 30, 128);
	SDL_RenderFillRect(renderer, &rect);
	SDL_RenderCopy(renderer, health, NULL, &rect);
}
