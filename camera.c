#include "survive.h"

SDL_Point camera;
unsigned int screenshake;

/*==========
MapToCamera
==========*/
SDL_Rect MapToCamera(SDL_Rect r)
{
	return (SDL_Rect) { r.x - camera.x, r.y - camera.y, r.w, r.h };
}

/*==========
LockCameraToMap

stop the camera going off the edges of the game world
==========*/
void LockCameraToMap(void)
{
	if (camera.x < 0)
		camera.x = 0;
	if (camera.y < 0)
		camera.y = 0;
	if (camera.x > map.width*TILE_WIDTH - windowmode.w)
		camera.x = map.width*TILE_WIDTH - windowmode.w;
	if (camera.y > map.height*TILE_HEIGHT - windowmode.h)
		camera.y = map.height*TILE_HEIGHT - windowmode.h;
}

/*==========
ApplyScreenShake
==========*/
void ApplyScreenShake(void)
{
	static unsigned int time;

	// the global "screenshake" is set to time + x when something explodes
	if (screenshake > thistime)
	{
		static int shake;
		if (time < thistime)
		{
			shake = shake > 0 ? -1 : 1;
			time = thistime + 50;
		}
		camera.x += shake;
		camera.y += shake;
		LockCameraToMap();
	}
	else
		time = 0;
}

/*==========
LockCameraToPlayer

focus the camera on the player at all times
==========*/
void LockCameraToPlayer(void)
{
	if (player)
	{
		camera.x = player->pos.x + (player->pos.w / 2) - (windowmode.w / 2);
		camera.y = player->pos.y + (player->pos.h / 2) - (windowmode.h / 2);
	}
	LockCameraToMap();
	ApplyScreenShake();
}
