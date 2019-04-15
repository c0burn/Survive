#include "survive.h"

unsigned int thistime, lasttime;
double deltatime;

/*==========
AnimateEnt
==========*/
void AnimateEnt(Entity* e)
{
	if (e->removed)
		return;

	int w, h;
	SDL_QueryTexture(e->tex, NULL, NULL, &w, &h);
	if (w % e->framewidth)
		printf("Width of texture does not match framewidth!\n");
	if (h % e->frameheight)
		printf("Height of texture does not match frameheight!\n");

	unsigned int across = w / e->framewidth;
	unsigned int down = h / e->frameheight;

	// a single frame, so update it immediately
	if (e->minframe == e->maxframe)
	{
		e->frame = e->minframe;
		int xpos = e->frame % across;
		int ypos = e->frame / across;
		e->texrect.x = xpos * e->framewidth;
		e->texrect.y = ypos * e->frameheight;
		return;
	}

	if (e->animtime > thistime)
		return;
	e->animtime = thistime + e->animspeed;

	if (e->frame < e->minframe)
		e->frame = e->minframe;
	else
	{
		e->frame = e->frame + 1;
		if (e->frame > e->maxframe)
			e->frame = e->minframe;
	}

	int xpos = e->frame % across;
	int ypos = e->frame / across;
	e->texrect.x = xpos * e->framewidth;
	e->texrect.y = ypos * e->frameheight;
}

/*==========
UpdateEnts
==========*/
void UpdateEnts(void)
{
	Entity* e = entities;
	while (e)
	{
		if (e->removed)
		{
			e = e->next;
			continue;
		}
		if (e->think)
		{
			if (thistime > e->thinktime && e->thinktime > 0)
			{
				e->thinktime = 0; 
				(*e->think)(e);
			}
		}
		EntPhysics(e);
		AnimateEnt(e);
		e = e->next;
	}

	// remove any entities that have been flagged for removal
	e = entities;
	while (e)
	{
		Entity* nextent = e->next;
		if (e->removed)
			DeleteEntity(e);
		e = nextent;
	}
}

/*=========
ToggleFullScreen
=========*/
void ToggleFullScreen(void)
{
	fullscreen = !fullscreen;
	if (fullscreen)
	{
		int err = SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		if (err < 0)
		{
			printf("%s\n", SDL_GetError());
			fullscreen = false;
			return;
		}
		AddMessage("Fullscreen on", GREEN);
		SDL_WarpMouseGlobal(desktopmode.w / 2, desktopmode.h / 2);
	}
	else
	{
		int err = SDL_SetWindowFullscreen(window, 0);
		if (err < 0)
		{
			printf("%s\n", SDL_GetError());
			fullscreen = true;
			return;
		}
		SDL_WarpMouseInWindow(window, windowmode.w / 2, windowmode.h / 2);
		AddMessage("Fullscreen off", GREEN);
	}
}

/*=========
HandleEvents
=========*/
void HandleEvents(bool* quit)
{	
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT) // alt-f4, close button, etc
			*quit = true;
		if (e.type == SDL_KEYDOWN)
		{
			if (e.key.keysym.sym == SDLK_ESCAPE)
				*quit = true;
			else if (e.key.keysym.sym == SDLK_RETURN && SDL_GetModState() & KMOD_ALT)
				ToggleFullScreen();
			else if (e.key.keysym.sym == SDLK_F1)
			{
				RenderDebug = !RenderDebug;
				if (RenderDebug)
					AddMessage("Debug rendering on", RED);
				else
					AddMessage("Debug rendering off", RED);
			}
		}
		if (e.type == SDL_MOUSEMOTION)
		{
			mousepos.x = e.motion.x;
			mousepos.y = e.motion.y;
		}
	}
}

void DebugControls(void)
{
	if (keys[SDL_SCANCODE_A])
		camera.x -= 1;
	else if (keys[SDL_SCANCODE_D])
		camera.x += 1;
	if (keys[SDL_SCANCODE_W])
		camera.y -= 1;
	else if (keys[SDL_SCANCODE_S])
		camera.y += 1;
}

/*=========
GameLoop
=========*/
void GameLoop(void)
{
	bool quit = false;
	while (!quit)
	{
		thistime = SDL_GetTicks(); // time in milliseconds since SDL library was initialised
		if (thistime - lasttime < 1)
		{
			Render();
			continue;
		}
		deltatime = (double)(thistime - lasttime) / 1000;
		lasttime = thistime;
		
		// we only update the mouse buttons here - mouse position is updated in HandleEvents due to issues with SDL logical size
		mousebuttons = SDL_GetMouseState(NULL, NULL); 
		HandleEvents(&quit);
		if (quit)
			break;
		
		PurgeMessages(); 
		AnimateParticles();
		UpdateEnts(); 
		PlayerControls();
		Render();
	}
}
