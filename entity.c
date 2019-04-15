#include "survive.h"

Entity* entities;

/*=========
AddEntity

entities are a linked list, the last created entity is pointed to by "entities"
you can traverse the linked list with ->next and ->prev

returns a pointer to the newly created entity - or exits on failure
========*/
Entity* AddEntity(EntityType type)
{
	Entity* e = calloc(1, sizeof(Entity));
	if (e == NULL)
	{
		perror("AddEntity: calloc failed");
		exit(EXIT_FAILURE);
	}

	e->type = type;
	if (entities != NULL)
		entities->prev = e;
	e->next = entities;
	entities = e;
	return e;
}

/*=========
DeleteEntity
========*/
void DeleteEntity(Entity* ent)
{
	if (ent == NULL)
	{
		printf("DeleteEntity: passed null pointer\n");
		return;
	}
	if (entities == NULL)
	{
		printf("DeleteEntity: entities is null\n");
		return;
	}

	Entity* before = ent->next;
	Entity* after = ent->prev;
	if (before != NULL)
		before->prev = after;
	if (after != NULL)
		after->next = before;
	if (ent == entities)
		entities = ent->next;
	free(ent);
}

/*==========
ClearEntities
==========*/
void ClearEntities(void)
{
	Entity* e = entities;
	while (e)
	{
		Entity* next = e->next;
		free(e);
		e = next;
	}
	entities = NULL;
}
