#include "survive.h"

/*==========
DestroyOldestMessage
==========*/
void DestroyOldestMessage(void)
{
	// destroy the texture
	if (Messages.msg[0].texture != NULL)
	{
		SDL_DestroyTexture(Messages.msg[0].texture);
		Messages.msg[0].texture = NULL;
	}
	//Messages.msg[0].colour = WHITE;
	//memset(Messages.msg[0].text, 0, sizeof(Messages.msg[0].text));

	// move messages up
	//for (unsigned int i = 0; i < Messages.count; i++)
		//Messages.msg[i] = Messages.msg[i + 1];
	Messages.msg[0] = Messages.msg[1];
	Messages.msg[1] = Messages.msg[2];
	Messages.msg[2] = Messages.msg[3];
}

/*==========
PurgeMessages
==========*/
void PurgeMessages(void)
{
	if (!Messages.count)
		return;
	if (thistime < Messages.purgetime)
		return;
	Messages.purgetime = thistime + 1000;
	DestroyOldestMessage();
	Messages.count--;
}

/*==========
AddMessage
==========*/
void AddMessage(const char* str, SDL_Color colour)
{
	Messages.purgetime = thistime + 1000;
	if (Messages.count == NUM_MESSAGES)
		DestroyOldestMessage();
	else
		Messages.count++;

	Message* m = &Messages.msg[Messages.count - 1];
	snprintf(m->text, sizeof(m->text), "%s", str);
	m->colour = colour;
	m->texture = CreateTextureFromText(m->text, m->colour, TTF_STYLE_NORMAL);
}
