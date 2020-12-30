/* 
 * OpenTyrian: A modern cross-platform port of Tyrian
 * Copyright (C) 2007-2009  The OpenTyrian Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "xmas.h"

#include "font.h"
#include "joystick.h"
#include "keyboard.h"
#include "mouse.h"
#include "palette.h"
#include "sprite.h"
#include "video.h"

#include <stdio.h>
#include <time.h>

bool xmas = false;

bool xmas_time( void )
{
	time_t now = time(NULL);
	return localtime(&now)->tm_mon == 11;
}

bool xmas_prompt( void )
{
	static const char *const prompt[] =
	{
		"Christmas has been detected.",
		"Activate Christmas?",
	};
	static const char *const choices[] =
	{
		"Yes",
		"No",
	};
	
	if (shopSpriteSheet.data == NULL)
		JE_loadCompShapes(&shopSpriteSheet, '1');  // need mouse pointer sprites

	bool restart = true;

	const int xCenter = 320 / 2;
	const int yPrompt = 85;
	const int dyPrompt = 15;
	const int wChoice = 40;
	const int yChoice = 120;
	const int hChoice = 13;
	
	size_t selectedIndex = 0;
	
	for (; ; )
	{
		if (restart)
		{
			// Draw prompt.
			for (uint i = 0; i < COUNTOF(prompt); ++i)
				draw_font_hv(VGAScreen, xCenter, yPrompt + dyPrompt * i, prompt[i], normal_font, centered, (i % 2) ? 2 : 4, -2);
		}

		// Draw choices.
		for (size_t i = 0; i < COUNTOF(choices); ++i)
		{
			const int x = xCenter - wChoice / 2 + wChoice * (int)i;

			const bool selected = (selectedIndex == i);

			draw_font_hv(VGAScreen, x, yChoice, choices[i], normal_font, centered, 15, selected ? -2 : -4);
		}

		if (restart)
		{
			mouseCursor = MOUSE_POINTER_NORMAL;

			fade_palette(palettes[0], 10, 0, 255);

			restart = false;
		}

		service_SDL_events(true);

		JE_mouseStart();
		JE_showVGA();
		JE_mouseReplace();

		bool mouseMoved = false;
		do
		{
			SDL_Delay(16);

			Uint16 oldMouseX = mouse_x;
			Uint16 oldMouseY = mouse_y;

			push_joysticks_as_keyboard();
			service_SDL_events(false);

			mouseMoved = mouse_x != oldMouseX || mouse_y != oldMouseY;
		} while (!(newkey || newmouse || mouseMoved));

		// Handle interaction.

		bool action = false;
		bool cancel = false;

		if (mouseMoved || newmouse)
		{
			// Find choice that was hovered or clicked.
			if (mouse_y >= yChoice && mouse_y < yChoice + hChoice)
			{
				for (size_t i = 0; i < COUNTOF(choices); ++i)
				{
					const int xChoice = xCenter - wChoice + wChoice * (int)i;
					if (mouse_x >= xChoice && mouse_x < xChoice + wChoice)
					{
						selectedIndex = i;

						if (newmouse && lastmouse_but == SDL_BUTTON_LEFT &&
						    lastmouse_x >= xChoice && lastmouse_x < xChoice + wChoice &&
						    lastmouse_y >= yChoice && lastmouse_y < yChoice + hChoice)
						{
							action = true;
						}

						break;
					}
				}
			}
		}

		if (newmouse)
		{
			if (lastmouse_but == SDL_BUTTON_RIGHT)
			{
				cancel = true;
			}
		}
		else if (newkey)
		{
			switch (lastkey_scan)
			{
			case SDL_SCANCODE_LEFT:
			{
				selectedIndex = selectedIndex == 0
					? COUNTOF(choices) - 1
					: selectedIndex - 1;
				break;
			}
			case SDL_SCANCODE_RIGHT:
			{
				selectedIndex = selectedIndex == COUNTOF(choices) - 1
					? 0
					: selectedIndex + 1;
				break;
			}
			case SDL_SCANCODE_SPACE:
			case SDL_SCANCODE_RETURN:
			{
				action = true;
				break;
			}
			case SDL_SCANCODE_ESCAPE:
			{
				cancel = true;
				break;
			}
			default:
				break;
			}
		}

		if (action || cancel)
		{
			fade_black(10);

			return !cancel && selectedIndex == 0;
		}
	}
}
