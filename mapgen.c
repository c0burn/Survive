#include "survive.h"

#define ROOM_TRY		128
#define ROOM_MIN		8
#define ROOM_MAX		16
#define MAP_MIN			48
#define MAP_MAX			80

#define WEIGHT_FLOOR	1
#define WEIGHT_EMPTY	4
#define WEIGHT_WALL		20

Map	map;
SDL_Rect* rooms;
int roomcount;

enum WallType { WALL_NONE = 0, WALL_UP = 1, WALL_DOWN = 2, WALL_LEFT = 4, WALL_RIGHT = 8 };

/*==========
TileForPosition

returns a pointer to a Tile given its x and y position in the map
returns NULL if no such tile
==========*/
Tile* TileForPosition(int x, int y)
{
	if (x < 0)
		return NULL;
	if (x > map.width - 1)
		return NULL;
	if (y < 0)
		return NULL;
	if (y > map.height - 1)
		return NULL;
	return &map.tiles[y * map.width + x];
}

/*==========
GetTileType

returns the type of a Tile given its x and y position in the map
returns -1 if tile isn't valid
==========*/
TileType GetTileType(int x, int y)
{
	Tile* thistile = TileForPosition(x, y); 
	if (thistile == NULL)
		return -1;

	return thistile->type;
}

/*==========
AdjacentTiles

returns the number of tiles adjacent to a tile which have the type "whichtype"
tests all 8 surrounding tiles, not just left/right/up/down
if tile is not valid, 0 is returned
==========*/
int AdjacentTiles(Tile* whichtile, int whichtype)
{
	if (whichtile == NULL)
		return 0;

	int count = 0;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (i == 0 && j == 0) // ignore whichtile
				continue;

			TileType type = GetTileType(whichtile->tilepos.x + i, whichtile->tilepos.y + j);
			if (type == whichtype)
				count++;
		}
	}
	return count;
}

int CalculateWallType(int x, int y)
{
	int score = 0;

	TileType up = GetTileType(x, y - 1);
	TileType down = GetTileType(x, y + 1);
	TileType left = GetTileType(x - 1, y);
	TileType right = GetTileType(x + 1, y);

	if (up == TILE_WALL)
		score = score + WALL_UP;
	if (down == TILE_WALL)
		score = score + WALL_DOWN;
	if (left == TILE_WALL)
		score = score + WALL_LEFT;
	if (right == TILE_WALL)
		score = score + WALL_RIGHT;

	return score;
}

/*==========
CalculateTileTextures
==========*/
void CalculateTileTextures(void)
{
	for (int x = 0; x < map.width; x++)
	{
		for (int y = 0; y < map.height; y++)
		{
			Tile* thistile = &map.tiles[y * map.width + x];

			// empty
			if (thistile->type == TILE_EMPTY)
			{
				thistile->texpos = (SDL_Rect) { 48, 0, 16, 16 };
			}

			// wall
			if (thistile->type == TILE_WALL)
			{
				int w = CalculateWallType(x, y);

				if (w == WALL_NONE)
					thistile->texpos = (SDL_Rect) { 0, 128, 16, 16 };
				if (w == WALL_UP)
					thistile->texpos = (SDL_Rect) { 0, 176, 16, 16 };
				if (w == WALL_DOWN)
					thistile->texpos = (SDL_Rect) { 0, 144, 16, 16 };
				if (w == WALL_DOWN + WALL_UP)
					thistile->texpos = (SDL_Rect) { 0, 160, 16, 16 };
				if (w == WALL_LEFT)
					thistile->texpos = (SDL_Rect) { 48, 128, 16, 16 };
				if (w == WALL_LEFT + WALL_UP)
					thistile->texpos = (SDL_Rect) { 32, 160, 16, 16 };
				if (w == WALL_LEFT + WALL_DOWN)
					thistile->texpos = (SDL_Rect) { 32, 144, 16, 16 };
				if (w == WALL_LEFT + WALL_DOWN + WALL_UP)
					thistile->texpos = (SDL_Rect) { 32, 176, 16, 16 };
				if (w == WALL_RIGHT)
					thistile->texpos = (SDL_Rect) { 16, 128, 16, 16 };
				if (w == WALL_RIGHT + WALL_UP)
					thistile->texpos = (SDL_Rect) { 16, 160, 16, 16 };
				if (w == WALL_RIGHT + WALL_DOWN)
					thistile->texpos = (SDL_Rect) { 16, 144, 16, 16 };
				if (w == WALL_RIGHT + WALL_DOWN + WALL_UP)
					thistile->texpos = (SDL_Rect) { 48, 176, 16, 16 };
				if (w == WALL_RIGHT + WALL_LEFT)
					thistile->texpos = (SDL_Rect) { 32, 128, 16, 16 };
				if (w == WALL_RIGHT + WALL_LEFT + WALL_UP)
					thistile->texpos = (SDL_Rect) { 48, 160, 16, 16 };
				if (w == WALL_RIGHT + WALL_LEFT + WALL_DOWN)
					thistile->texpos = (SDL_Rect) { 32, 176, 16, 16 };
				if (w == WALL_RIGHT + WALL_LEFT + WALL_DOWN + WALL_UP)
					thistile->texpos = (SDL_Rect) { 48, 144, 16, 16 };
			}

			// floor
			if (thistile->type == TILE_FLOOR)
			{
				int r = rand() % 100;

				if (r < 5)
					thistile->texpos = (SDL_Rect) { 256, 128, 16, 16 };
				else if (r < 10)
					thistile->texpos = (SDL_Rect) { 256, 144, 16, 16 };
				else if (r < 15)
					thistile->texpos = (SDL_Rect) { 272, 128, 16, 16 };
				else
					thistile->texpos = (SDL_Rect) { 272, 144, 16, 16 };
			}
		}
	}
}

/*==========
FillHoles

ensure floor tiles always have a wall adjacent to them
==========*/
void FillHoles(void)
{
	for (int t = 0; t < map.width * map.height; t++)
	{
		Tile* thistile = &map.tiles[t];
		if (thistile->type == TILE_EMPTY)
		{
			if (AdjacentTiles(thistile, TILE_FLOOR))
				thistile->type = TILE_WALL;
		}
	}
}

/*==========
PlaceFloor
==========*/
void PlaceFloor(SDL_Rect room)
{
	for (int i = 0; i < room.w; i++)
	{
		for (int j = 0; j < room.h; j++)
		{
			Tile* thistile = TileForPosition(room.x + i, room.y + j);
			if (thistile != NULL)
			{
				thistile->weight = WEIGHT_FLOOR;
				thistile->type = TILE_FLOOR;
			}
		}
	}
}

/*==========
ClearRoutes
==========*/
void ClearRoutes(void)
{
	for (int i = 0; i < map.width * map.height; i++)
	{
		map.tiles[i].distance = -1;
		map.tiles[i].visited = false;
		map.tiles[i].prev = NULL;
	}
}

/*==========
CheckTile

update distance score for the adjacent tiles
==========*/
void CheckTile(int x, int y, Tile* currenttile)
{
	Tile* checktile = TileForPosition(x, y);
	if (checktile == NULL)
		return;
	if (checktile->visited)
		return;
	if (checktile->distance == -1 || checktile->distance < currenttile->distance)
	{
		checktile->distance = checktile->weight + currenttile->distance;
		checktile->prev = currenttile;
	}
}

/*==========
FindPath

generate a path from a start and end tile using A* routing
distance is scored by the weighting of each tile
we use 4=empty,1=floor,20=walls
this encourages the algorithm to use existing paths and only break through a wall if no other path exists
==========*/
void FindPath(Tile* start, Tile* end)
{
	// the first tile is always distance zero
	Tile* thistile = start; 
	thistile->distance = 0;

	while (1)
	{
		// mark this tile as visited
		thistile->visited = true;

		// we have reached the goal when it's been marked as visited
		if (end->visited)
		{
			printf("Found route!\n");

			// build a path from the goal working backwards to the start
			Tile* marker = end;
			while (marker)
			{
				SDL_Rect floor;
				int r = rand() % 100;
				if (r < 20)
				{
					floor.x = marker->tilepos.x - 1;
					floor.y = marker->tilepos.y - 1;
					floor.w = 3;
					floor.h = 3;
				}
				else if (r < 40)
				{
					floor.x = marker->tilepos.x;
					floor.y = marker->tilepos.y;
					floor.w = 2;
					floor.h = 2;
				}
				else if (r < 60)
				{
					floor.x = marker->tilepos.x - 1;
					floor.y = marker->tilepos.y - 1;
					floor.w = 2;
					floor.h = 2;
				}
				else
				{
					floor.x = marker->tilepos.x;
					floor.y = marker->tilepos.y;
					floor.w = floor.h = 1;
				}
				PlaceFloor(floor);
				marker = marker->prev;
			}
			return;
		}

		// check adjacent tiles and update their distance scores
		CheckTile(thistile->tilepos.x + 1, thistile->tilepos.y, thistile);
		CheckTile(thistile->tilepos.x - 1, thistile->tilepos.y, thistile);
		CheckTile(thistile->tilepos.x, thistile->tilepos.y + 1, thistile);
		CheckTile(thistile->tilepos.x, thistile->tilepos.y - 1, thistile);

		// find the best scored, unvisited tile
		Tile* best = NULL;
		int bdist = -1;
		for (int i = 0; i < map.width*map.height; i++)
		{
			Tile* check = &map.tiles[i];
			if (!check->visited && check->distance > 0)
			{
				if (check->distance < bdist || bdist == -1)
				{
					best = check;
					bdist = check->distance;
				}
			}
		}

		// if no next tile then there isn't a valid path
		// this should never occur ...
		if (best == NULL)
		{
			printf("No valid route exists!\n");
			return;
		}

		// best tile becomes the current tile on the next run around the loop
		thistile = best;
	}
}

/*==========
StartPath
==========*/
void StartPath(SDL_Rect start, SDL_Rect end)
{
	// mark all tiles as unvisited and reset distance scores
	ClearRoutes();

	// the start and end tiles are in the middle of each room
	Tile* start_tile = TileForPosition(start.x + (start.w / 2), start.y + (start.h / 2));
	if (start_tile == NULL)
	{
		printf("FindPath: start is NULL: ");
		printf("%d %d %d %d\n", start.x, start.y, start.w, start.h);
		return;
	}

	Tile* end_tile = TileForPosition(end.x + (end.w / 2), end.y + (end.h / 2));
	if (end_tile == NULL)
	{
		printf("FindPath: end is NULL: ");
		printf("%d %d %d %d\n", end.x, end.y, end.w, end.h);
		return;
	}

	FindPath(start_tile, end_tile);
}

/*===========
GeneratePaths
==========*/
void GeneratePaths(void)
{
	for (int i = 0; i < roomcount - 1; i++)
	{
		printf("StartPath: %i -> %i\n", i, i + 1);
		StartPath(rooms[i], rooms[i + 1]);
	}
}

/*==========
PlaceRoom
==========*/
void PlaceRoom(SDL_Rect room)
{
	for (int i = 0; i < room.w; i++)
	{
		for (int j = 0; j < room.h; j++)
		{
			Tile* thistile = TileForPosition(room.x + i, room.y + j);
			if (thistile != NULL)
			{
				// place walls at the edges of the room
				if (i == 0 || i == room.w - 1 || j == 0 || j == room.h - 1)
				{
					thistile->weight = WEIGHT_WALL;
					thistile->type = TILE_WALL;
				}
				// place floor on other tiles
				else
				{
					thistile->weight = WEIGHT_FLOOR;
					thistile->type = TILE_FLOOR;
				}
			}
		}
	}

	// punch some holes in the walls for the path-finding
	// we just pick a random spot on the wall that isn't a corner spot 
	int hole_top = rand_range(1, room.w - 2);
	int hole_bottom = rand_range(1, room.w - 2);
	int hole_left = rand_range(1, room.h - 2);
	int hole_right = rand_range(1, room.h - 2);

	Tile* thistile = TileForPosition(room.x + hole_top, room.y);
	if (thistile != NULL)
	{
		thistile->type = TILE_EMPTY;
		thistile->weight = WEIGHT_EMPTY;
	}

	thistile = TileForPosition(room.x + hole_bottom, room.y + room.h - 1);
	if (thistile != NULL)
	{
		thistile->type = TILE_EMPTY;
		thistile->weight = WEIGHT_EMPTY;
	}

	thistile = TileForPosition(room.x, room.y + hole_left);
	if (thistile != NULL)
	{
		thistile->type = TILE_EMPTY;
		thistile->weight = WEIGHT_EMPTY;
	}

	thistile = TileForPosition(room.x + room.w - 1, room.y + hole_right);
	if (thistile != NULL)
	{
		thistile->type = TILE_EMPTY;
		thistile->weight = WEIGHT_EMPTY;
	}
}

/*==========
RoomOverlaps

returns true if "room" overlaps any existing room in the array
==========*/
bool RoomOverlaps(SDL_Rect room)
{
	for (int i = 0; i < roomcount; i++)
	{
		if (SDL_HasIntersection(&room, &rooms[i]))
			return true;
	}
	return false;
}

/*==========
GenerateRooms
==========*/
void GenerateRooms(void)
{
	int try = ROOM_TRY;
	while (try)
	{
		try--;

		// create a randomly sized and positioned room
		SDL_Rect room;
		room.w = rand_range(ROOM_MIN, ROOM_MAX);
		room.h = rand_range(ROOM_MIN, ROOM_MAX);
		room.x = rand_range(0, map.width - room.w - 1);
		room.y = rand_range(0, map.height - room.h - 1);

		// check if the new room overlaps an existing room
		if (RoomOverlaps(room))
			continue;

		// add the new room to the map
		SDL_Rect* temp = realloc(rooms, ++roomcount * sizeof(SDL_Rect));
		if (temp == NULL) 
		{
			perror("GenerateRooms: realloc failed");
			free(rooms);
			exit(EXIT_FAILURE);
		}
		else
		{
			rooms = temp;
			temp = NULL;
		}
		rooms[roomcount - 1] = room;
		PlaceRoom(room);
		printf("Placed room %d: %d %d %d %d\n", roomcount - 1, room.x, room.y, room.w, room.h);
	}

	printf("Placed %i rooms\n", roomcount);
}

/*==========
GenerateMiniMap
==========*/
void GenerateMiniMap(void)
{
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	// create a surface we will later fill with the minimap pixel data
	SDL_Surface* minimap_surf = SDL_CreateRGBSurface(0, map.width, map.height, 32, rmask, gmask, bmask, amask);
	if (minimap_surf == NULL)
	{
		printf("%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// fill the surface with some pixels
	SDL_PixelFormat* fmt = minimap_surf->format;
	Uint32* pixels = (Uint32*)minimap_surf->pixels;
	for (int x = 0; x < minimap_surf->w; x++)
	{
		for (int y = 0; y < minimap_surf->h; y++)
		{
			// blue floors and red walls
			if (map.tiles[y * minimap_surf->w + x].type == TILE_FLOOR)
				pixels[(y * minimap_surf->w) + x] = SDL_MapRGBA(fmt, 0, 0, 128, 255);
			else if (map.tiles[y * minimap_surf->w + x].type == TILE_WALL)
				pixels[(y * minimap_surf->w) + x] = SDL_MapRGBA(fmt, 192, 0, 0, 255);
			else
				pixels[(y * minimap_surf->w) + x] = SDL_MapRGBA(fmt, 30, 30, 30, 128);
		}
	}

	// destroy any existing minimap texture
	if (map.minimap != NULL)
	{
		SDL_DestroyTexture(map.minimap);
		map.minimap = NULL;
	}

	// create the texture from the above surface, and then free the surface
	map.minimap = SDL_CreateTextureFromSurface(renderer, minimap_surf);
	if (map.minimap == NULL)
	{
		printf("%s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_FreeSurface(minimap_surf);
}

/*==========
CalculateTilePositions

we fill in each tile with some basic information about its position in the world
all tiles are empty by default
==========*/
void CalculateTilePositions(void)
{
	for (int x = 0; x < map.width; x++)
	{
		for (int y = 0; y < map.height; y++)
		{
			Tile* thistile = &map.tiles[(y*map.width) + x];
			thistile->worldpos.x = x * TILE_WIDTH;
			thistile->worldpos.y = y * TILE_HEIGHT;
			thistile->worldpos.w = TILE_WIDTH;
			thistile->worldpos.h = TILE_HEIGHT;
			thistile->tilepos.x = x;
			thistile->tilepos.y = y;
			thistile->type = TILE_EMPTY;
			thistile->weight = WEIGHT_EMPTY;
		}
	}
}

/*==========
ClearMap

clear all map data
==========*/
void ClearMap(void)
{
	if (map.tiles)
		free(map.tiles);
	if (map.tex)
		SDL_DestroyTexture(map.tex);
	if (map.minimap)
		SDL_DestroyTexture(map.minimap);

	map.tiles = NULL;
	map.tex = NULL;
	map.minimap = NULL;
	map.width = 0;
	map.height = 0;
}

/*==========
CreateNewMap

create a new, randomly sized map
==========*/
void CreateNewMap(void)
{
	ClearMap();

	map.tex = LoadTexture("sprites/tiles.png");
	if (!map.tex)
	{
		printf("CreateNewMap: failed to load tiles\n");
		exit(EXIT_FAILURE);
	}

	map.width = rand_range(MAP_MIN, MAP_MAX);
	map.height = rand_range(MAP_MIN, MAP_MAX);
	printf("Generating new map... %i x %i\n", map.width, map.height);
	map.tiles = calloc(map.width * map.height, sizeof(Tile));
	if (!map.tiles)
	{
		perror("CreateNewMap: calloc failed\n");
		exit(EXIT_FAILURE);
	}

	CalculateTilePositions();
	GenerateRooms();
	GeneratePaths();
	FillHoles();
	CalculateTileTextures();
	GenerateMiniMap();
}
