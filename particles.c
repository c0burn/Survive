#include "survive.h"

typedef struct Particle {
	SDL_Rect		pos;
	SDL_Rect		texrect;
	SDL_Texture*	tex;
	double			angle;
	unsigned int	frame, minframe, maxframe, framewidth;
	unsigned int	animtime, animspeed;
	int				alpha, alphafade;
	unsigned int	dietime;
	bool			removed, loop;
	SDL_BlendMode	blend;

	// linked list
	struct Particle* next;
	struct Particle* prev;
} Particle;

Particle* particles;

/*=========
AddParticle
========*/
Particle* AddParticle(void)
{
	Particle* p = calloc(1, sizeof(Particle));
	if (p == NULL)
	{
		perror("AddParticle: calloc failed");
		exit(EXIT_FAILURE);
	}

	if (particles != NULL)
		particles->prev = p;
	p->next = particles;
	particles = p;
	return p;
}

/*=========
DeleteParticle
========*/
void DeleteParticle(Particle* p)
{
	if (p == NULL)
	{
		printf("DeleteParticle: passed null pointer\n");
		return;
	}
	if (particles == NULL)
	{
		printf("DeleteParticle: particles is null\n");
		return;
	}

	Particle* before = p->next;
	Particle* after = p->prev;
	if (before != NULL)
		before->prev = after;
	if (after != NULL)
		after->next = before;
	if (p == particles)
		particles = p->next;
	free(p);
}

/*==========
AnimateParticles
==========*/
void AnimateParticles(void)
{
	if (!particles)
		return;
	Particle* p = particles;
	while (p)
	{
		// particles are removed when dietime has passed
		if (p->dietime > 0 && p->dietime < thistime)
		{
			p->removed = true;
			p = p->next;
			continue;
		}
		// not time for this particle to animate
		if (p->animtime > thistime)
		{
			p = p->next;
			continue;
		}

		// if minframe and maxframe are set, the particle will animate between them
		// if loop is true, dietime must also be set, or the particle will never be removed
		if (p->minframe < p->maxframe)
		{
			p->frame = p->frame + 1;
			if (p->frame > p->maxframe)
			{
				if (!p->loop)
				{
					p->removed = true;
					p = p->next;
					continue;
				}
				p->frame = p->minframe;
			}
		}
		else
			p->frame = p->minframe;

		// alpha fade out
		p->alpha -= p->alphafade;
		if (p->alpha <= 0)
		{
			p->removed = true;
			p = p->next;
			continue;
		}

		// set new animation frame
		p->animtime = thistime + p->animspeed;
		p->texrect.x = p->frame * p->framewidth;
		p = p->next;
	}

	// delete any particles that were marked for removal
	p = particles;
	while (p)
	{
		Particle* nextp = p->next;
		if (p->removed)
			DeleteParticle(p);
		p = nextp;
	}
}

/*==========
SpawnSpark
==========*/
void SpawnSpark(SDL_Point pos)
{
	SDL_Texture* sparktex = LoadTexture("sprites/sparks.png");

	Particle* s = AddParticle();
	s->tex = sparktex;
	s->blend = SDL_BLENDMODE_ADD;
	s->texrect = (SDL_Rect) { 0, 0, 48, 48 };

	s->pos.x = pos.x - 24;
	s->pos.y = pos.y - 24;
	s->pos.w = s->pos.h = 48;
	
	s->minframe = 0;
	s->maxframe = 14;
	s->frame = 0;
	s->framewidth = 48;
	s->animspeed = 50;
	s->alpha = 200;
	s->alphafade = 16;
	s->animtime = thistime + s->animspeed;
}

/*==========
SpawnExplosion
==========*/
void SpawnExplosion(SDL_Point pos)
{
	SDL_Texture* explosion_texture = LoadTexture("sprites/explosion.png");

	Particle* s = AddParticle();
	s->tex = explosion_texture;
	s->blend = SDL_BLENDMODE_ADD;
	s->texrect = (SDL_Rect) { 0, 0, 64, 64 };

	s->pos.x = pos.x - 32;
	s->pos.y = pos.y - 32;
	s->pos.w = s->pos.h = 64;

	s->minframe = 0;
	s->maxframe = 31;
	s->frame = 0;
	s->framewidth = 64;
	s->animspeed = 50;
	s->alpha = 255;
	s->alphafade = 2;
	s->animtime = thistime + s->animspeed;
}

/*==========
SpawnBlood
==========*/
void SpawnBlood(SDL_Point pos)
{
	SDL_Texture* blood_texture = LoadTexture("sprites/blood.png");

	for (int i = 0; i < 16; i++)
	{
		Particle* p = AddParticle();

		p->angle = rand_range(0, 359);
		
		p->tex = blood_texture;
		p->blend = SDL_BLENDMODE_BLEND;
		p->texrect = (SDL_Rect) { 0, 0, 32, 32 };

		p->pos.x = pos.x - 16;
		p->pos.y = pos.y - 16;

		p->pos.x += rand() % 32;
		p->pos.y += rand() % 32;

		p->pos.w = p->pos.h = 16;

		int r = rand_range(0, 3);
		p->minframe = r * 5;
		p->maxframe = p->minframe + 3;

		p->frame = p->minframe;
		p->framewidth = 32;
		p->animspeed = 80;
		p->alpha = 128;
		//p->alphafade = 16;
		//p->dietime = thistime + 5000;
		p->animtime = thistime + p->animspeed;
	}
}

/*==========
RenderParticles
==========*/
void RenderParticles(void)
{
	Particle* p = particles;
	while (p)
	{
		SDL_Rect r1 = p->pos;
		SDL_Rect r2 = (SDL_Rect) { camera.x, camera.y, windowmode.w, windowmode.h };
		if (!SDL_HasIntersection(&r1, &r2))
		{
			p = p->next;
			continue;
		}

		SDL_Rect rect = MapToCamera(p->pos);
		SDL_SetTextureBlendMode(p->tex, p->blend);
		SDL_SetTextureAlphaMod(p->tex, (Uint8)p->alpha);
		SDL_RenderCopyEx(renderer, p->tex, &p->texrect, &rect, p->angle, NULL, SDL_FLIP_NONE);
		p = p->next;
	}
}
