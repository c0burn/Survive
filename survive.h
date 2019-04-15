#ifdef _MSC_VER 
#pragma warning( disable : 4204 )
#endif

/*
  Includes
*/

#include <stdio.h>
#include <stdlib.h>  
#include <stdbool.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

/*
 Defines
*/

#define SCREEN_WIDTH		800
#define SCREEN_HEIGHT		600
#define TILE_WIDTH			32
#define TILE_HEIGHT			32
#define TEXT_SIZE			8
#define NUM_MESSAGES		4

#define PLAYER_ACCEL		1024
#define PLAYER_FRICTION		1024
#define PLAYER_SPEED		128
#define PLAYER_WIDTH		48
#define PLAYER_HEIGHT		48
#define PLAYER_COLLISION	16,16,32,32

#define WHITE (SDL_Color)	{ 255,255,255 }
#define BLACK (SDL_Color)	{ 0,0,0 }
#define GREY (SDL_Color)	{ 128,128,128 }
#define RED (SDL_Color)		{ 255,0,0 }
#define GREEN (SDL_Color)	{ 0,255,0 }
#define BLUE (SDL_Color)	{ 0,0,255 }
#define CYAN (SDL_Color)	{ 0,255,255 }
#define MAGENTA (SDL_Color)	{ 255,0,255 }
#define YELLOW (SDL_Color)	{ 255,255,0 }

/*
  Enums
*/

typedef enum EntityType { ENTITY_PLAYER, ENTITY_BULLET, ENTITY_BUG } EntityType;
typedef enum TileType { TILE_EMPTY, TILE_WALL, TILE_FLOOR } TileType;
typedef enum Direction { UP, DOWN, LEFT, RIGHT } Direction;

/*
  Structs
*/

typedef struct Vecd { double x, y; } Vecd;

typedef struct Entity {
	EntityType		type; 
	bool			removed;

	// physics / collisions
	struct Entity*	owner;
	SDL_Rect		pos, collision, collision_offset;
	Vecd			move, speed, accum;
	bool			blocked_x, blocked_y;
	
	// textures and animations
	SDL_Texture*	tex;
	SDL_Rect		texrect; 
	unsigned int	frame, minframe, maxframe,
					framewidth, frameheight,
					animtime, animspeed;
	Direction	facing;

	// rendering flags
	double				angle;
	unsigned int		glowtime;
	SDL_Color			glowcolor;
	SDL_RendererFlip	flip;

	// think and touch functions
	unsigned int	thinktime; 
	void			(*think)(struct Entity*);
	void			(*touch)(struct Entity*, struct Entity*);

	// linked list
	struct Entity*	next;
	struct Entity*	prev;
} Entity;

typedef struct CollisionResult {
	bool collided;
	Entity* ent;
} CollisionResult;

typedef struct Tile {
	TileType		type;
	SDL_Rect		worldpos;
	SDL_Point		tilepos;
	SDL_Rect		texpos;
	int				weight;
	int				distance;
	bool			visited;
	struct Tile*	prev;
} Tile;

typedef struct Map {
	int				width, height;
	SDL_Texture*	tex;
	SDL_Texture*	minimap;
	Tile*			tiles;
} Map;

typedef struct Message {
	char			text[256];
	SDL_Color		colour;
	SDL_Texture*	texture;
} Message;

struct Messages {
	Message			msg[NUM_MESSAGES];
	unsigned int	count;
	unsigned int	purgetime;
} Messages;

/*
  Globals
*/

extern SDL_Window*		window;
extern SDL_Renderer*	renderer;
extern SDL_Texture*		target;
extern SDL_Cursor*		cursor;
extern TTF_Font*		font;

extern SDL_Point		camera;
extern unsigned int		screenshake;

extern Entity*			player;
extern Entity*			entities;
extern unsigned int		thistime, lasttime, animtime;
extern double			deltatime;
extern bool				fullscreen;
extern SDL_DisplayMode	desktopmode;
extern SDL_DisplayMode	windowmode;
extern Uint32 			mousebuttons;
extern SDL_Point		mousepos;
extern const Uint8*		keys;
extern Map				map;
extern bool				RenderDebug;
extern SDL_Rect*		rooms;
extern int				roomcount;

extern Mix_Chunk* sfx_sword;

/*
  Function Prototypes
*/

// camera.c
SDL_Rect MapToCamera(SDL_Rect r); 
void LockCameraToMap(void);
void LockCameraToPlayer(void);

// entity.c
Entity* AddEntity(EntityType type);
void DeleteEntity(Entity* ent);
void ClearEntities(void);

// gameloop.c
void CreateNewMap(void);
void GameLoop(void);

// mapgen.c
Tile* TileForPosition(int x, int y);
void ClearMap(void);

// menu.c
void SetupMenu(void);

// messages.c
void PurgeMessages(void);
void AddMessage(const char* str, SDL_Color colour);

// misc.c
double crand(void);
int rand_range(int min, int max);
int clamp(int what, int min, int max);
double radtodeg(double rad);
double degtorad(double deg);
double anglemod(double ang);

// phys.c
void EntPhysics(Entity* ent);
SDL_Point TraceCollision(SDL_Point start, SDL_Point end);
CollisionResult CheckCollision(SDL_Rect testrect, Entity* ignore);
SDL_Rect CalcCollisionRect(Entity* ent);

// particles.c
void SpawnSpark(SDL_Point pos);
void RenderParticles(void);
void AnimateParticles(void);

// player.c
void PlayerControls(void);
void PlayerRun(Entity* self);

// render.c
void Render(void);

// texture.c
SDL_Texture* CreateTextureFromText(const char* str, SDL_Color colour, int style);
SDL_Texture* CreateCroppedTexture(const char* path, SDL_Rect rect);
SDL_Texture* LoadTexture(const char* path);
