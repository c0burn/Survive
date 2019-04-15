#include "survive.h"

Entity*	player;

/*==========
PlayerRun
==========*/
void PlayerRun(Entity* self)
{
	self->flip = SDL_FLIP_NONE;
	self->animspeed = 100;

	// moving, but not blocked
	if ((self->move.x && !self->blocked_x) || (self->move.y && !self->blocked_y))
	{
		if (self->move.y < 0)
		{
			self->minframe = 16;
			self->maxframe = 19;
		}
		else if (self->move.y > 0)
		{
			self->minframe = 8;
			self->maxframe = 11;
		}
		else if (self->move.x)
		{
			// left uses the right frames but flipped horizontally
			if (self->move.x < 0)
				self->flip = SDL_FLIP_HORIZONTAL;
			self->minframe = 12;
			self->maxframe = 15;
		}
	}
	// still, or blocked
	else
	{
		if (self->facing == DOWN)
		{
			self->minframe = 0;
			self->maxframe = 2;
			self->animspeed = 300;
		}
		else if (self->facing == RIGHT)
			self->minframe = self->maxframe = 39;
		else if (self->facing == LEFT)
		{
			self->minframe = self->maxframe = 39;
			self->flip = SDL_FLIP_HORIZONTAL;
		}
		else if (self->facing == UP)
			self->minframe = self->maxframe = 43;
	}

	self->think = &PlayerRun;
	self->thinktime = thistime + 100;
}

/*==========
PlayerShoot
==========*/
/*static void PlayerShoot(void)
{
	static unsigned int firetime;
	if (firetime > thistime)
		return;
	firetime = thistime + 100;
	player->glowtime = thistime + 50;


	SDL_Point spawnpos;
	spawnpos.x = player->pos.x + (player->pos.w / 2);
	spawnpos.y = player->pos.y + (player->pos.h / 2);
	double angle = degtorad(player->angle);
	spawnpos.x += (int)(12 * cos(angle) - 4 * sin(angle));
	spawnpos.y += (int)(12 * sin(angle) + 4 * cos(angle));

	int count = 1;
	while (count)
	{
		double randangle = angle + crand() * (M_PI / 30);

		SDL_Point endpos = (SDL_Point) { (int)(spawnpos.x + cos(randangle) * 1024), (int)(spawnpos.y + sin(randangle) * 1024) };
		SDL_Point foo = TraceCollision(spawnpos, endpos);
		SpawnSpark(foo);
		//SpawnBlood(foo);
		count--;
	}
}*/

/*==========
BulletThink
==========*/
void BulletThink(Entity* self)
{
	SpawnSpark((SDL_Point) { self->pos.x + self->pos.w / 2, self->pos.y + self->pos.h / 2 });
	self->thinktime = thistime + 20;
}

void BulletTouch(Entity* self, Entity* other)
{
	if (other)
	{
		if (other->type == ENTITY_BUG)
		{
			other->removed = true;
			SpawnBlood((SDL_Point) { self->pos.x, self->pos.y });
		}
	}


	//SpawnExplosion((SDL_Point){ self->pos.x, self->pos.y });
	screenshake = thistime + 100;

	//if (other == NULL)
		self->removed = true;
}

/*
void PlayerShoot(void)
{
	static unsigned int firetime;
	if (firetime > thistime)
		return;
	firetime = thistime + 100;
	player->glowtime = thistime + 100;

	// create new entity for bullet
	Entity* bullet = AddEntity(ENTITY_BULLET);
	bullet->owner = player;
	bullet->think = &BulletThink;
	bullet->thinktime = thistime + 20;
	bullet->touch = &BulletTouch;

	SDL_Texture* btex = LoadTexture("sprites/bullet.png");

	bullet->tex = btex;
	bullet->texrect = (SDL_Rect) { 0, 0, 22, 10 };
	bullet->collision_offset = (SDL_Rect) { 0, 0, 22, 10 };

	// position bullet at gun barrel
	SDL_Rect spawnpos;
	spawnpos.w = 22;
	spawnpos.h = 10;
	spawnpos.x = player->pos.x + (player->pos.w / 2) - (spawnpos.w / 2);
	spawnpos.y = player->pos.y + (player->pos.h / 2) - (spawnpos.h / 2);
	
	double angle = degtorad(player->angle);
	spawnpos.x += (int)(16 * cos(angle));
	spawnpos.y += (int)(16 * sin(angle));

	//spawnpos.x += (int)(12 * cos(angle) - 4 * sin(angle));
	//spawnpos.y += (int)(12 * sin(angle) + 4 * cos(angle));

	bullet->pos = spawnpos;
	bullet->collision = CalcCollisionRect(bullet);
	bullet->angle = radtodeg(angle);
	bullet->move.x = cos(angle) * 512;
	bullet->move.y = sin(angle) * 512;
}
*/

void PlayerShootPistol(void)
{
	SDL_Point spawnpos;
	spawnpos.x = player->pos.x + (player->pos.w / 2);
	spawnpos.y = player->pos.y + (player->pos.h / 2);
	double angle = degtorad(player->angle);
	spawnpos.x += (int)(12 * cos(angle) - 4 * sin(angle));
	spawnpos.y += (int)(12 * sin(angle) + 4 * cos(angle));

	int count = 8;
	while (count)
	{
		double randangle = angle + crand() * 5 * (M_PI / 180);
		//double randangle = angle;
		SDL_Point endpos = (SDL_Point) { (int)(spawnpos.x + cos(randangle) * 1024), (int)(spawnpos.y + sin(randangle) * 1024) };
		SDL_Point foo = TraceCollision(spawnpos, endpos);
		SpawnSpark(foo);
		count--;
	}
}

void PlayerShoot4(Entity* self)
{
	self->minframe = 23;
	if (player->facing == RIGHT)
		player->minframe += 4;
	if (player->facing == LEFT)
	{
		player->minframe += 4;
		player->flip = SDL_FLIP_HORIZONTAL;
	}
	if (player->facing == UP)
		player->minframe += 8;
	player->maxframe = player->minframe;

	self->think = &PlayerRun;
	self->thinktime = thistime + 100;
}

void PlayerShoot3(Entity* self)
{
	self->minframe = 22;
	if (player->facing == RIGHT)
		player->minframe += 4;
	if (player->facing == LEFT)
	{
		player->minframe += 4;
		player->flip = SDL_FLIP_HORIZONTAL;
	}
	if (player->facing == UP)
		player->minframe += 8;
	player->maxframe = player->minframe;

	self->think = &PlayerShoot4;
	self->thinktime = thistime + 100;
}

void PlayerShoot2(Entity* self)
{
	self->minframe = 21;
	if (player->facing == RIGHT)
		player->minframe += 4;
	if (player->facing == LEFT)
	{
		player->minframe += 4;
		player->flip = SDL_FLIP_HORIZONTAL;
	}
	if (player->facing == UP)
		player->minframe += 8;
	player->maxframe = player->minframe;

	self->think = &PlayerShoot3;
	self->thinktime = thistime + 100;
}

void PlayerShoot(void)
{
	Mix_PlayChannel(-1, sfx_sword, 0);

	player->minframe = 20;
	if (player->facing == RIGHT)
		player->minframe += 4;
	if (player->facing == LEFT)
	{
		player->minframe += 4;
		player->flip = SDL_FLIP_HORIZONTAL;
	}
	if (player->facing == UP)
		player->minframe += 8;
	player->maxframe = player->minframe;

	player->think = &PlayerShoot2;
	player->thinktime = thistime + 100;
}

/*==========
PlayerControls
==========*/
void PlayerControls(void)
{
	static unsigned int firetime;

	if (!player)
		return;

	// rotate the player to face mouse position 
	//SDL_Point world = (SDL_Point) { mousepos.x + camera.x, mousepos.y + camera.y };
	//player->angle = radtodeg(atan2(world.y - (player->pos.y + player->pos.h / 2), world.x - (player->pos.x + player->pos.w / 2)));
	//player->angle = anglemod(player->angle);

	player->move.x = 0;
	player->move.y = 0;
	
	// handle player movement keys
	if (firetime > thistime)
		return;

	// check for left mouse
	if (mousebuttons & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		firetime = thistime + 400;
		PlayerShoot();
		return;
	}

	if (keys[SDL_SCANCODE_A])
	{
		player->move.x = -1;
		player->facing = LEFT;
	}
	else if (keys[SDL_SCANCODE_D])
	{
		player->move.x = 1;
		player->facing = RIGHT;
	}
	if (keys[SDL_SCANCODE_W])
	{
		player->move.y = -1;
		player->facing = UP;
	}
	else if (keys[SDL_SCANCODE_S])
	{
		player->move.y = 1;
		player->facing = DOWN;
	}

	// update player animation
	// PlayerAnimate();
}
