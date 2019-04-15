#include "survive.h"

/*==========
SpawnPlayer
==========*/
void SpawnPlayer(void)
{
	player = AddEntity(ENTITY_PLAYER);
	player->tex = LoadTexture("sprites/hero.png");
	player->pos = (SDL_Rect) { 0, 0, 96, 96 };
	player->collision_offset = (SDL_Rect) { 32, 32, 32, 32 };
	player->texrect = (SDL_Rect) { 0, 0, 48, 48 };
	player->animspeed = 100;
	player->animtime = thistime + player->animspeed;
	player->framewidth = player->frameheight = 48;
	player->think = &PlayerRun;
	player->thinktime = thistime + 100;

	// position the player
	while (1)
	{
		// pick a random room and attempt to put the player in the middle of it
		int whichroom = rand_range(0, roomcount - 1);
		player->pos.x = (int)(TILE_WIDTH * rooms[whichroom].x + (TILE_WIDTH * rooms[whichroom].w * 0.5) - (PLAYER_WIDTH * 0.5));
		player->pos.y = (int)(TILE_HEIGHT * rooms[whichroom].y + (TILE_HEIGHT* rooms[whichroom].h * 0.5) - (PLAYER_HEIGHT * 0.5));
		player->collision = CalcCollisionRect(player);

		CollisionResult result = CheckCollision(player->collision, player);
		if (!result.collided)
			break;

		printf("Player was blocked\n");
	}
}

void BugThink(Entity* self)
{
	int r = rand() % 100;

	if (r < 33)
		self->move.x = 64;
	else if (r < 66)
		self->move.x = -64;
	else
		self->move.x = 0;

	r = rand() % 100;
	if (r < 33)
		self->move.y = 64;
	else if (r < 66)
		self->move.y = -64;
	else
		self->move.y = 0;

	if (self->move.x == 0 && self->move.y == 0)
		self->minframe = self->maxframe = 0;
	else
		self->maxframe = 7;

	if (fabs(self->move.x) > 0 || fabs(self->move.y) > 0)
		self->angle = radtodeg(atan2(self->move.y, self->move.x));

	self->thinktime = thistime + 1000 + (rand() % 1000);
}

void SpawnBugs(void)
{
	Entity* bug;
	SDL_Texture* bugtexture = LoadTexture("sprites/bug.png");

	int count = 0;
	int try = 512;

	while (try)
	{
		try--;

		bug = AddEntity(ENTITY_BUG);
		bug->tex = bugtexture;
		bug->texrect = (SDL_Rect) { 0, 0, 64, 64 };
		bug->collision_offset = (SDL_Rect) { 16, 16, 32, 32 };

		int x = rand_range(0, map.width - 1);
		int y = rand_range(0, map.height - 1);
		bug->pos.x = 48 * x;
		bug->pos.y = 48 * y;
		bug->pos.w = 64;
		bug->pos.h = 64;
		bug->collision = CalcCollisionRect(bug);

		CollisionResult result = CheckCollision(bug->collision, bug);
		if (result.collided)
			DeleteEntity(bug);
		else
			count++;

		bug->think = &BugThink;
		bug->thinktime = thistime + 20;
		bug->animspeed = 100;
		bug->minframe = 0;
		bug->maxframe = 3;
		bug->framewidth = 64;

		if (count >= 256)
			break;
	}

	printf("Spawned %i bugs\n", count);
}

/*==========
SpawnEntities
==========*/
void SpawnEntities(void)
{
	SpawnPlayer();
//	SpawnBugs();
}