#include "survive.h"

#define MINIMAP_OFFSET 16

bool RenderDebug;

/*==========
OnScreen
==========*/
bool OnScreen(SDL_Rect rect)
{
	SDL_Rect compare = (SDL_Rect) { camera.x, camera.y, windowmode.w, windowmode.h };
	if (SDL_HasIntersection(&rect, &compare))
		return true;
	return false;
}

/*=========
RenderMiniMap
=========*/
void RenderMiniMap(void)
{
	// the minimap is drawn at the top right of the screen
	SDL_Rect rect = { windowmode.w - map.width*2 - MINIMAP_OFFSET, MINIMAP_OFFSET, map.width*2, map.height*2 };
	SDL_RenderCopy(renderer, map.minimap, NULL, &rect);

	if (!player)
		return;

	// draw player position
	rect.x += 2 * ((player->pos.x + (player->pos.w / 2)) / TILE_WIDTH);
	rect.y += 2 * ((player->pos.y + (player->pos.h / 2)) / TILE_HEIGHT);
	rect.w = rect.h = 2;
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderFillRect(renderer, &rect);
}

/*==========
RenderEntities
==========*/
void RenderEntities(void)
{
	Entity* e = entities;
	while (e)
	{
		if (!OnScreen(e->pos))
		{
			e = e->next;
			continue;
		}

		SDL_Rect rect = MapToCamera(e->pos);
		if (e->glowtime > thistime)
		{
			SDL_SetTextureBlendMode(e->tex, SDL_BLENDMODE_BLEND);
			SDL_SetTextureColorMod(e->tex, e->glowcolor.r, e->glowcolor.g, e->glowcolor.b);
			SDL_RenderCopyEx(renderer, e->tex, &e->texrect, &rect, e->angle, NULL, e->flip);

			SDL_SetTextureBlendMode(e->tex, SDL_BLENDMODE_ADD);
			SDL_SetTextureColorMod(e->tex, e->glowcolor.r, e->glowcolor.g, e->glowcolor.b);
			SDL_RenderCopyEx(renderer, e->tex, &e->texrect, &rect, e->angle, NULL, e->flip);

			SDL_SetTextureBlendMode(e->tex, SDL_BLENDMODE_BLEND);
			SDL_SetTextureColorMod(e->tex, 255, 255, 255);
		}
		else
		{
			SDL_RenderCopyEx(renderer, e->tex, &e->texrect, &rect, e->angle, NULL, e->flip);
		}

		// draw debugging bounding boxes
		if (RenderDebug)
		{
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 192);
			SDL_RenderDrawRect(renderer, &rect);

			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 192);
			rect = MapToCamera(e->collision);
			SDL_RenderDrawRect(renderer, &rect);
		}

		e = e->next;
	}
}

/*==========
RenderMap
==========*/
void RenderMap(void)
{
	if (!map.tiles)
		return;

	for (int i = 0; i < map.width * map.height; i++)
	{
		Tile* thistile = &map.tiles[i];
		if (!OnScreen(thistile->worldpos))
			continue;
		SDL_Rect rect = MapToCamera(thistile->worldpos);
		SDL_RenderCopy(renderer, map.tex, &thistile->texpos, &rect);
	}
}

/*==========
RenderMessages
==========*/
void RenderMessages(void)
{
	if (!Messages.count)
		return;

	// messages are rendered at the top left of the screen
	SDL_Rect rect;
	rect.x = 16;
	rect.y = 16;

	for (unsigned int i = Messages.count; i > 0; i--)
	{
		Message* m = &Messages.msg[i - 1];
		//SDL_QueryTexture(m->texture, NULL, NULL, &rect.w, &rect.h);

		rect.w = strlen(m->text) * TEXT_SIZE * 2;
		rect.h = TEXT_SIZE * 2;

		// draw a background for easier visibility of text
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0, 0, 30, 128);
		SDL_RenderFillRect(renderer, &rect);

		// draw the text texture
		SDL_SetTextureBlendMode(m->texture, SDL_BLENDMODE_BLEND);
		// highlight first message
		if (i == Messages.count)
			SDL_SetTextureAlphaMod(m->texture, 255);
		else
			SDL_SetTextureAlphaMod(m->texture, 192);
		SDL_RenderCopy(renderer, m->texture, NULL, &rect);
		rect.y += rect.h;
	}
}

/*=========
Render
=========*/
void Render(void)
{
	// we render to a target texture, then draw the whole texture to screen
	SDL_SetRenderTarget(renderer, target);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	LockCameraToPlayer();
	RenderMap();
	RenderEntities();
	RenderParticles();
	RenderMessages();
	RenderMiniMap();
	//RenderHud();
	//RenderMenu();

	SDL_SetRenderTarget(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer); 
	SDL_RenderCopy(renderer, target, NULL, NULL);
	
	/*
	static double fade;
	if (fade >= 128)
		fade += (deltatime * 64);
	else
		fade += (deltatime * 32);
	if (fade >= 255)
		fade = 255;
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, (int)(255 - fade));
	SDL_RenderFillRect(renderer, NULL);
	*/

	SDL_RenderPresent(renderer); 
}
