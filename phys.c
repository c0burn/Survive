#include "survive.h"

/*==========
CalcCollisionRect
==========*/
SDL_Rect CalcCollisionRect(Entity* ent)
{
	SDL_Rect result = ent->pos;
	// sanity check
	if (ent->collision_offset.w == 0 || ent->collision_offset.h == 0)
	{
		printf("CalcCollisionRect: width or height is zero!\n");
		return result;
	}
	result.x += ent->collision_offset.x;
	result.y += ent->collision_offset.y; 
	result.w = ent->collision_offset.w;
	result.h = ent->collision_offset.h;
	return result;
}

/*==========
CheckPointCollisionWithEdges

check a point to see if it's off the edge of the map
==========*/
bool CheckPointCollisionWithEdges(SDL_Point p)
{
	if (p.x < 0) return true;
	if (p.x > map.width * TILE_WIDTH) return true;
	if (p.y < 0) return true;
	if (p.y > map.height * TILE_HEIGHT)	return true;
	return false;
}

/*==========
CheckPointCollisionWithTiles

check a point to see if it collides with a tile
==========*/
bool CheckPointCollisionWithTiles(SDL_Point p)
{
	Tile* thistile = TileForPosition(p.x / TILE_WIDTH, p.y / TILE_HEIGHT);
	if (thistile == NULL)
		return true;
	if (thistile->type == TILE_FLOOR)
		return false;
	return true;
}

/*==========
CheckPointCollisionWithEntities

check a point to see if it collides with an entity
==========*/
bool CheckPointCollisionWithEntities(SDL_Point p)
{
	Entity* e = entities;
	while (e)
	{
		if (e == player || e->type == ENTITY_BULLET)
		{
			e = e->next;
			continue;
		}
		if (SDL_PointInRect(&p, &e->collision))
			return true;
		e = e->next;
	}
	return false;
}

/*==========
TraceCollision
==========*/
SDL_Point TraceCollision(SDL_Point start, SDL_Point end)
{
	int dx = abs(end.x - start.x);
	int sx = start.x < end.x ? 1 : -1;
	int dy = -abs(end.y - start.y);
	int sy = start.y < end.y ? 1 : -1;
	int err = dx + dy;

	SDL_Point last = start;
	while (true)
	{
		if (CheckPointCollisionWithEdges(start))
			return last;
		if (CheckPointCollisionWithTiles(start))
			return last;
		if (CheckPointCollisionWithEntities(start))
			return last;

		// reached end
		if (start.x == end.x && start.y == end.y)
			return start;

		last = start;
		int err2 = 2 * err;
		if (err2 >= dy) { err += dy; start.x += sx; }
		if (err2 <= dx) { err += dx; start.y += sy; }
	}
}

/*==========
CheckCollisionWithEdges
==========*/
bool CheckCollisionWithEdges(SDL_Rect testrect)
{
	if (testrect.x < 0)
		return true;
	if (testrect.x + testrect.w > map.width * TILE_WIDTH)
		return true;
	if (testrect.y < 0)
		return true;
	if (testrect.y + testrect.h > map.height * TILE_HEIGHT)
		return true;
	return false;
}

/*==========
CheckCollisionWithTiles
==========*/
bool CheckCollisionWithTiles(SDL_Rect testrect)
{
	SDL_Point topleft, bottomright;

	topleft.x = (testrect.x / TILE_WIDTH) - 1;
	topleft.y = (testrect.y / TILE_HEIGHT) - 1;
	topleft.x = clamp(topleft.x, 0, map.width - 1);
	topleft.y = clamp(topleft.y, 0, map.height - 1);

	bottomright.x = ((testrect.x + testrect.w) / TILE_WIDTH) + 1;
	bottomright.y = ((testrect.y + testrect.h) / TILE_WIDTH) + 1;
	bottomright.x = clamp(bottomright.x, 0, map.width - 1);
	bottomright.y = clamp(bottomright.y, 0, map.height - 1);

	int hoz = bottomright.x - topleft.x;
	int vert = bottomright.y - topleft.y;
	for (int i = 0; i <= hoz; i++)
	{
		for (int j = 0; j <= vert; j++)
		{
			Tile* thistile = &map.tiles[((topleft.y + j) * map.width) + topleft.x + i];
			if (thistile->type == TILE_FLOOR)
				continue;
			if (SDL_HasIntersection(&testrect, &thistile->worldpos))
				return true;
		}
	}

	return false;
}

/*==========
CheckCollisionWithEntities
=========*/
Entity* CheckCollisionWithEntities(SDL_Rect testrect, Entity* ignore)
{
	Entity* e = entities;
	while (e)
	{
		// don't collide with self, or our own projectiles
		if (e == ignore || ignore->owner == e || e->owner == ignore)
		{
			e = e->next;
			continue;
		}
		if (SDL_HasIntersection(&testrect, &e->collision))
		{
			return e;
		}
		e = e->next;
	}
	return NULL;
}

/*==========
CheckCollision
==========*/
CollisionResult CheckCollision(SDL_Rect testrect, Entity* ignore)
{
	// we need to return two values:
	// whether a collision occured,
	// and what we hit
	CollisionResult result;
	result.collided = false;
	result.ent = NULL;

	if (CheckCollisionWithEdges(testrect))
	{
		result.collided = true;
		return result;
	}
	if (CheckCollisionWithTiles(testrect))
	{
		result.collided = true;
		return result;
	}
	Entity* temp = CheckCollisionWithEntities(testrect, ignore);
	if (temp)
	{
		result.collided = true;
		result.ent = temp;
		return result;
	}
	return result;
}

SDL_Rect AdjustRect(SDL_Rect r1, SDL_Rect r2)
{
	return (SDL_Rect) { r1.x + r2.x, r1.y + r2.y, r1.w + r2.w, r1.h + r2.h };
}

bool EntityTryMove(Entity* ent, int units, SDL_Rect direction)
{
	if (ent->removed)
		return false;

	SDL_Rect testrect = ent->collision;
	while (units)
	{
		testrect = AdjustRect(testrect, direction);
		CollisionResult result = CheckCollision(testrect, ent);
		if (!result.collided)
		{
			// we successfully moved
			ent->pos = AdjustRect(ent->pos, direction);
			ent->collision = AdjustRect(ent->collision, direction);
			units--;
		}
		else
		{
			if (ent->touch)
				ent->touch(ent, result.ent);
			return false;
		}
	}
	return true;
}

/*==========
EntMove
==========*/
void EntMove(Entity* ent)
{
	if (ent->removed)
		return;

	// accumulate our speed until it's big enough to attempt to move 1 unit
	ent->accum.x = ent->speed.x ? ent->accum.x + ent->speed.x * deltatime : 0;
	ent->accum.y = ent->speed.y ? ent->accum.y + ent->speed.y * deltatime : 0;

	// test is the number of world units (i.e. pixels) to attempt to move
	// nudge is the direction of movement to attempt
	SDL_Point test = (SDL_Point) { (int)trunc(ent->accum.x), (int)trunc(ent->accum.y) };
	SDL_Point nudge = (SDL_Point) { 0, 0 };
	ent->accum.x -= test.x;
	nudge.x = test.x > 0 ? 1 : -1;
	ent->accum.y -= test.y;
	nudge.y = test.y > 0 ? 1 : -1;

	if (test.x)
	{
		if (EntityTryMove(ent, abs(test.x), (SDL_Rect) { nudge.x, 0, 0, 0 }))
			ent->blocked_x = false;
		else
		{
			ent->blocked_x = true;
			ent->speed.x = ent->accum.x = 0;
		}
	}

	if (test.y)
	{
		if (EntityTryMove(ent, abs(test.y), (SDL_Rect) { 0, nudge.y, 0, 0 }))
			ent->blocked_y = false;
		else
		{
			ent->blocked_y = true;
			ent->speed.y = ent->accum.y = 0;
		}
	}

	// horizontal
	/*
	SDL_Rect testrect = ent->collision;
	while (test.x)
	{
		testrect.x += nudge.x;
		CollisionResult result = CheckCollision(testrect, ent);
		if (!result.collided)
		{
			// we successfully moved left/right
			ent->blocked_x = false;
			ent->pos.x += nudge.x;
			ent->collision.x += nudge.x;
			test.x -= nudge.x;
		}
		else
		{
			ent->blocked_x = true;
			ent->speed.x = ent->accum.x = test.x = 0;
			if (ent->touch)
				ent->touch(ent, result.e);
		}
	}

	if (ent->removed)
		return;

	// vertical
	testrect = ent->collision;
	while (test.y)
	{
		testrect.y += nudge.y;
		CollisionResult result = CheckCollision(testrect, ent);
		if (!result.collided)
		{
			// we successfully moved up/down
			ent->blocked_y = false;
			ent->pos.y += nudge.y;
			ent->collision.y += nudge.y;
			test.y -= nudge.y;
		}
		else
		{
			ent->blocked_y = true;
			ent->speed.y = ent->accum.y = test.y = 0;
			if (ent->touch)
				ent->touch(ent, result.e);
		}
	}
	*/
}

void PlayerPhysics(Entity* ent)
{
//	ent->speed.x = ent->move.x ? ent->move.x * PLAYER_SPEED : 0;
//	ent->speed.y = ent->move.y ? ent->move.y * PLAYER_SPEED : 0;

	// apply acceleration
	if (ent->move.x)
		ent->speed.x += ent->move.x * PLAYER_ACCEL * deltatime;
	if (ent->move.y)
		ent->speed.y += ent->move.y * PLAYER_ACCEL * deltatime;

	// cap max speed
	if (ent->speed.x > PLAYER_SPEED)
		ent->speed.x = PLAYER_SPEED;
	if (ent->speed.x < -PLAYER_SPEED)
		ent->speed.x = -PLAYER_SPEED;
	if (ent->speed.y > PLAYER_SPEED)
		ent->speed.y = PLAYER_SPEED;
	if (ent->speed.y < -PLAYER_SPEED)
		ent->speed.y = -PLAYER_SPEED;

	// apply friction
	if (fabs(ent->speed.x) > 0 && ent->move.x == 0)
	{
		if (ent->speed.x < 0)
		{
			ent->speed.x += PLAYER_FRICTION * deltatime;
			if (ent->speed.x > 0)
				ent->speed.x = 0;
		}
		else
		{
			ent->speed.x -= PLAYER_FRICTION * deltatime;
			if (ent->speed.x < 0)
				ent->speed.x = 0;
		}
	}

	if (fabs(ent->speed.y) > 0 && ent->move.y == 0)
	{
		if (ent->speed.y < 0)
		{
			ent->speed.y += PLAYER_FRICTION * deltatime;
			if (ent->speed.y > 0)
				ent->speed.y = 0;
		}
		else
		{
			ent->speed.y -= PLAYER_FRICTION * deltatime;
			if (ent->speed.y < 0)
				ent->speed.y = 0;
		}
	}
}

/*==========
ObjectPhysics
==========*/
void ObjectPhysics(Entity* ent)
{
	ent->speed.x = ent->move.x ? ent->move.x : 0;
	ent->speed.y = ent->move.y ? ent->move.y : 0;
}

/*==========
EntPhysics
==========*/
void EntPhysics(Entity* ent)
{
	if (ent == player)
		PlayerPhysics(ent);
	else
		ObjectPhysics(ent);

	EntMove(ent);
}
