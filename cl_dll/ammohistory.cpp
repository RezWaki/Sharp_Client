/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
//  ammohistory.cpp
//


#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>
#include <ctime>

#include "ammohistory.h"

HistoryResource gHR;

#define AMMO_PICKUP_GAP (gHR.iHistoryGap+5)
#define AMMO_PICKUP_PICK_HEIGHT		(32 + (gHR.iHistoryGap * 2))
#define AMMO_PICKUP_HEIGHT_MAX		(ScreenHeight - 100)

#define MAX_ITEM_NAME	32
int HISTORY_DRAW_TIME = 5;

// keep a list of items
struct ITEM_INFO
{
	char szName[MAX_ITEM_NAME];
	SpriteHandle_t spr;
	wrect_t rect;
};

INT iCurrentId = 0;
std::string pAllowed;

void HistoryResource :: AddToHistory( int iType, int iId, int iCount )
{
	if ( iType == HISTSLOT_AMMO && !iCount )
		return;  // no amount, so don't add

	if ( (((AMMO_PICKUP_GAP * iCurrentHistorySlot) + AMMO_PICKUP_PICK_HEIGHT) > AMMO_PICKUP_HEIGHT_MAX) || (iCurrentHistorySlot >= MAX_HISTORY) )
	{	// the pic would have to be drawn too high
		// so start from the bottom
		iCurrentHistorySlot = 0;
	}

	HIST_ITEM *freeslot = &rgAmmoHistory[iCurrentHistorySlot++];  // default to just writing to the first slot
	HISTORY_DRAW_TIME = CVAR_GET_FLOAT( "hud_drawhistory_time" );

	if( CVAR_GET_FLOAT("cl_istimer") && CVAR_GET_FLOAT("cl_istimer_wpns")
	&& iType == HISTSLOT_WEAP ) {
		CVAR_GET_FLOAT("cl_istimer_mode") ? gHUD.m_ItemSpawnTimer.iLastUsedIndex = 0
		: gHUD.m_ItemSpawnTimer.iLastUsedIndex++;
		if( gHUD.m_ItemSpawnTimer.iLastUsedIndex > CVAR_GET_FLOAT("cl_istimer") )
			gHUD.m_ItemSpawnTimer.iLastUsedIndex = 0;
		while( gHUD.m_ItemSpawnTimer.pShouldDrawTimer[gHUD.m_ItemSpawnTimer.iLastUsedIndex] ) {
			gHUD.m_ItemSpawnTimer.iLastUsedIndex++;
			if( gHUD.m_ItemSpawnTimer.iLastUsedIndex > CVAR_GET_FLOAT("cl_istimer") ) {
				gHUD.m_ItemSpawnTimer.iLastUsedIndex = 0;
				break;
			}
		}
		if( !gHUD.m_ItemSpawnTimer.pShouldDrawTimer[gHUD.m_ItemSpawnTimer.iLastUsedIndex] ) {
			gHUD.m_ItemSpawnTimer.pTimerSeconds[gHUD.m_ItemSpawnTimer.iLastUsedIndex] = 21.0; //weapon 20 sec
			gHUD.m_ItemSpawnTimer.pShouldDrawTimer[gHUD.m_ItemSpawnTimer.iLastUsedIndex] = TRUE;
			gHUD.m_ItemSpawnTimer.bFirst[gHUD.m_ItemSpawnTimer.iLastUsedIndex] = TRUE;
			strcpy( gHUD.m_ItemSpawnTimer.pItemNames[gHUD.m_ItemSpawnTimer.iLastUsedIndex], (char*)(gWR.GetWeapon(iId)->szName+7) );
		}
	}

	freeslot->type = iType;
	freeslot->iId = iId;
	freeslot->iCount = iCount;
	freeslot->DisplayTime = gHUD.m_flTime + HISTORY_DRAW_TIME;
}

void HistoryResource :: AddToHistory( int iType, const char *szName, int iCount )
{
	if ( iType != HISTSLOT_ITEM )
		return;

	if ( (((AMMO_PICKUP_GAP * iCurrentHistorySlot) + AMMO_PICKUP_PICK_HEIGHT) > AMMO_PICKUP_HEIGHT_MAX) || (iCurrentHistorySlot >= MAX_HISTORY) )
	{	// the pic would have to be drawn too high
		// so start from the bottom
		iCurrentHistorySlot = 0;
	}

	HIST_ITEM *freeslot = &rgAmmoHistory[iCurrentHistorySlot++];  // default to just writing to the first slot

	// I am really unhappy with all the code in this file

	int i = gHUD.GetSpriteIndex( szName );
	if ( i == -1 )
		return;  // unknown sprite name, don't add it to history

	if( CVAR_GET_FLOAT("cl_istimer") && CVAR_GET_FLOAT("cl_istimer_items") ) {
		CVAR_GET_FLOAT("cl_istimer_mode") ? gHUD.m_ItemSpawnTimer.iLastUsedIndex = 0
			: gHUD.m_ItemSpawnTimer.iLastUsedIndex++;
		if( gHUD.m_ItemSpawnTimer.iLastUsedIndex > CVAR_GET_FLOAT("cl_istimer") )
			gHUD.m_ItemSpawnTimer.iLastUsedIndex = 0;
		while( gHUD.m_ItemSpawnTimer.pShouldDrawTimer[gHUD.m_ItemSpawnTimer.iLastUsedIndex] ) {
			gHUD.m_ItemSpawnTimer.iLastUsedIndex++;
			if( gHUD.m_ItemSpawnTimer.iLastUsedIndex > CVAR_GET_FLOAT("cl_istimer") ) {
				gHUD.m_ItemSpawnTimer.iLastUsedIndex = 0;
				break;
			}
		}
		if( !gHUD.m_ItemSpawnTimer.pShouldDrawTimer[gHUD.m_ItemSpawnTimer.iLastUsedIndex] ) {
			gHUD.m_ItemSpawnTimer.pTimerSeconds[gHUD.m_ItemSpawnTimer.iLastUsedIndex] = 31.0; //item 30 sec
			gHUD.m_ItemSpawnTimer.pShouldDrawTimer[gHUD.m_ItemSpawnTimer.iLastUsedIndex] = TRUE;
			gHUD.m_ItemSpawnTimer.bFirst[gHUD.m_ItemSpawnTimer.iLastUsedIndex] = TRUE;
			strcpy( gHUD.m_ItemSpawnTimer.pItemNames[gHUD.m_ItemSpawnTimer.iLastUsedIndex], szName+5 );
		}
	}

	freeslot->iId = i;
	freeslot->type = iType;
	freeslot->iCount = iCount;

	HISTORY_DRAW_TIME = CVAR_GET_FLOAT( "hud_drawhistory_time" );
	freeslot->DisplayTime = gHUD.m_flTime + HISTORY_DRAW_TIME;
}


void HistoryResource :: CheckClearHistory( void )
{
	for ( int i = 0; i < MAX_HISTORY; i++ )
	{
		if ( rgAmmoHistory[i].type )
			return;
	}

	iCurrentHistorySlot = 0;
}

int m_flTurnoff = 0;
//int iHudTimerPos = ScreenHeight/6;

int hudColors_random[5][3] = {85, 0, 255,
							  255, 0, 0,
							  0, 255, 0,
							  255, 255, 0,
							  255, 255, 255};

//
// Draw Ammo pickup history
//
int HistoryResource :: DrawAmmoHistory( float flTime )
{
	for ( int i = 0; i < MAX_HISTORY; i++ )
	{
		r = gHUD.pHudColors[0]; g = gHUD.pHudColors[1]; b = gHUD.pHudColors[2];
		if ( rgAmmoHistory[i].type )
		{
			rgAmmoHistory[i].DisplayTime = min( rgAmmoHistory[i].DisplayTime, gHUD.m_flTime + HISTORY_DRAW_TIME );

			if ( rgAmmoHistory[i].DisplayTime <= flTime )
			{  // pic drawing time has expired
				memset( &rgAmmoHistory[i], 0, sizeof(HIST_ITEM) );
				CheckClearHistory();
			}
			else if ( rgAmmoHistory[i].type == HISTSLOT_AMMO )
			{
				wrect_t rcPic;
				SpriteHandle_t *spr = gWR.GetAmmoPicFromWeapon( rgAmmoHistory[i].iId, rcPic );

				//int r, g, b;
				//UnpackRGB(r,g,b, RGB_YELLOWISH);
				//float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				//ScaleColors(r, g, b, min(scale, 255) );

				// Draw the pic
				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i));
				int xpos = ScreenWidth - 24;
				if ( spr && *spr )    // weapon isn't loaded yet so just don't draw the pic
				{ // the dll has to make sure it has sent info the weapons you need
					SPR_Set( *spr, r, g, b );
					SPR_DrawAdditive( 0, xpos, ypos, &rcPic );
				}

				// Draw the number
				if( CVAR_GET_FLOAT("cl_rainbowhud") > 0 ) {
					srand( time(NULL) );
					int tempcolor = rand()%5;
					r = hudColors_random[tempcolor][0];
					g = hudColors_random[tempcolor][1];
					b = hudColors_random[tempcolor][2];
				}

				gHUD.DrawHudNumberString( xpos - 10, ypos, xpos - 100, rgAmmoHistory[i].iCount, r, g, b );
			}
			else if ( rgAmmoHistory[i].type == HISTSLOT_WEAP )
			{
				WEAPON *weap = gWR.GetWeapon( rgAmmoHistory[i].iId );

				if ( !weap )
					return 1;  // we don't know about the weapon yet, so don't draw anything

				if( CVAR_GET_FLOAT("cl_rainbowhud") == 2 ) {
					srand( time(NULL) );
					int tempcolor = rand()%3;
					r = hudColors_random[tempcolor][0];
					g = hudColors_random[tempcolor][1];
					b = hudColors_random[tempcolor][2];
				}

				if ( !gWR.HasAmmo( weap ) && !CVAR_GET_FLOAT("cl_rainbowhud") )
					UnpackRGB( r, g, b, RGB_REDISH );	// if the weapon doesn't have ammo, display it as red

				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i));
				int xpos = ScreenWidth - (weap->rcInactive.right - weap->rcInactive.left);
				SPR_Set( weap->hInactive, r, g, b );
				SPR_DrawAdditive( 0, xpos, ypos, &weap->rcInactive );
			}
			else if ( rgAmmoHistory[i].type == HISTSLOT_ITEM )
			{
				if ( !rgAmmoHistory[i].iId )
					continue;  // sprite not loaded

				wrect_t rect = gHUD.GetSpriteRect( rgAmmoHistory[i].iId );

				if( CVAR_GET_FLOAT("cl_rainbowhud") == 2 ) {
					srand( time(NULL) );
					int tempcolor = rand()%3;
					r = hudColors_random[tempcolor][0];
					g = hudColors_random[tempcolor][1];
					b = hudColors_random[tempcolor][2];
				}

				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i));
				int xpos = ScreenWidth - (rect.right - rect.left) - 10;

				SPR_Set( gHUD.GetSprite( rgAmmoHistory[i].iId ), r, g, b );
				SPR_DrawAdditive( 0, xpos, ypos, &rect );
			}
		}
	}

	return 1;
}