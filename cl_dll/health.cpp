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
// Health.cpp
//
// implementation of CHudHealth class
//

#include "STDIO.H"
#include "STDLIB.H"
#include "MATH.H"

#include "sharpfuncs.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"
#include "r_studioint.h"
#include "screenfade.h"
#include <string.h>
#include <ctime>

extern BOOL bCrosshairMustBeRed;
extern BOOL bAmISpec;
extern char* pSpecTargetName;
extern int pShouldDrawTimer;
extern int bShouldShowScoreboard;
extern float pTimerSeconds;
extern int pTeamColors[5][3];
//extern char* g_iSpecName;
//extern Vector dmg_vec;
//extern int iHudTimerPos;
extern CRenderFuncs gpRenderFuncs;
extern CFonts gpFonts;
extern extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS+1];
extern engine_studio_api_t IEngineStudio;

extern void V_PunchAxis( int axis, float punch );

DECLARE_MESSAGE(m_Health, Health )
DECLARE_MESSAGE(m_Health, Damage )

#define PAIN_NAME "sprites/%d_pain.spr"
#define DAMAGE_NAME "sprites/%d_dmg.spr"

int gNumberOfTeamColors = 5;
int gTeamColors[5][3] =
{
	{ 255, 170, 0 },	// HL orange (default)
	{ 125, 165, 210 },	// Blue
	{ 200, 90, 70 },	// Red
	{ 225, 205, 45 },	// Yellow
	{ 145, 215, 140 },	// Green
};

int giDmgHeight, giDmgWidth;

int giDmgFlags[NUM_DMG_TYPES] = 
{
	DMG_POISON,
	DMG_ACID,
	DMG_FREEZE|DMG_SLOWFREEZE,
	DMG_DROWN,
	DMG_BURN|DMG_SLOWBURN,
	DMG_NERVEGAS, 
	DMG_RADIATION,
	DMG_SHOCK,
	DMG_CALTROP,
	DMG_TRANQ,
	DMG_CONCUSS,
	DMG_HALLUC
};

int CHudHealth::Init(void)
{
	HOOK_MESSAGE(Health);
	HOOK_MESSAGE(Damage);
	m_iHealth = 100;
	m_fFade = 0;
	m_iFlags = 0;
	m_bitsDamage = 0;
	m_fAttackFront = m_fAttackRear = m_fAttackRight = m_fAttackLeft = 0;
	giDmgHeight = 0;
	giDmgWidth = 0;

	memset(m_dmg, 0, sizeof(DAMAGE_IMAGE) * NUM_DMG_TYPES);


	gHUD.AddHudElem(this);

	this->bMustBeDrawn = TRUE; //we must draw it in any way, even if the stars are falling

	return 1;
}

void CHudHealth::Reset( void )
{
	// make sure the pain compass is cleared when the player respawns
	m_fAttackFront = m_fAttackRear = m_fAttackRight = m_fAttackLeft = 0;


	// force all the flashing damage icons to expire
	m_bitsDamage = 0;
	for ( int i = 0; i < NUM_DMG_TYPES; i++ )
	{
		m_dmg[i].fExpire = 0;
	}

	m_iFlags &= ~HUD_ACTIVE;

}

int CHudHealth::VidInit(void)
{
	m_SpriteHandle_t = 0;

	m_HUD_dmg_bio = gHUD.GetSpriteIndex( "dmg_bio" ) + 1;
	m_HUD_cross = gHUD.GetSpriteIndex( "cross" );

	giDmgHeight = gHUD.GetSpriteRect(m_HUD_dmg_bio).right - gHUD.GetSpriteRect(m_HUD_dmg_bio).left;
	giDmgWidth = gHUD.GetSpriteRect(m_HUD_dmg_bio).bottom - gHUD.GetSpriteRect(m_HUD_dmg_bio).top;
	return 1;
}

int CHudHealth:: MsgFunc_Health(const char *pszName,  int iSize, void *pbuf )
{
	// TODO: update local health data
	BEGIN_READ( pbuf, iSize );
	int x = READ_BYTE();

	m_iFlags = HUD_ACTIVE;

	// Only update the fade if we've changed health
	if (x != m_iHealth)
	{
		m_fFade = FADE_TIME;
		m_iHealth = x;
	}

	return 1;
}


int CHudHealth:: MsgFunc_Damage(const char *pszName,  int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int armor = READ_BYTE();	// armor
	int damageTaken = READ_BYTE();	// health
	long bitsDamage = READ_LONG(); // damage bits

	vec3_t vecFrom;

	for ( int i = 0 ; i < 3 ; i++)
		vecFrom[i] = READ_COORD();

	UpdateTiles(gHUD.m_flTime, bitsDamage);

	// Actually took damage?
	if ( damageTaken > 0 || armor > 0 ) {
		//gpRenderFuncs.DrawDamageTrace( vecFrom );
		CalcDamageDirection(vecFrom);
	}


	return 1;
}


// Returns back a color from the
// Green <-> Yellow <-> Red ramp
void CHudHealth::GetPainColor( int &r, int &g, int &b )
{
	int iHealth = m_iHealth;

	if (iHealth > 25)
		iHealth -= 25;
	else if ( iHealth < 0 )
		iHealth = 0;
#if 0
	g = iHealth * 255 / 100;
	r = 255 - g;
	b = 0;
#else
	if (m_iHealth > 25)
	{
		//UnpackRGB(r,g,b, RGB_YELLOWISH);
	}
	else
	{
		r = 250;
		g = 0;
		b = 0;
	}
	sscanf(CVAR_GET_STRING("cl_hudcolor"), "%i %i %i", &r, &g, &b );
#endif 
}

char pFrags[256], pTeamInfo[256], pTimerInfo[256], pRealTimer[64];
INT pCrossColors[3], pHudColors[4], pSpecPlayers[2], iStartHeight[2] = { 1, 1 };
INT pTimerStrLen = 0;
FLOAT pTimerPos[2] = { 0, 0 };
FLOAT iClTime = 0.0, bFirst = 1, m_flRealTimer = 0.0;
SpriteHandle_t sBackgroundSprite, sPlayerFlag;
hud_player_info_t* pPlayerInfo[2];
std::string pTempWeapon;
time_t tRealTime; tm* realtime;

int CHudHealth::Draw(float flTime)
{
	int r, g, b;
	int a = 0, x, y;
	int HealthWidth;

	if( CVAR_GET_FLOAT("cl_clocks") ) {
		if( gEngfuncs.GetClientTime() > m_flRealTimer ) {
			tRealTime = time( NULL );
			realtime = localtime( &tRealTime );
			//fucking hell below
			sprintf( pRealTimer, CVAR_GET_FLOAT("cl_clocks_fmt") ? "%02i%s%02i%s%02i" : "%02i%s%02i%s%02i %s",
				CVAR_GET_FLOAT("cl_clocks_fmt") ? realtime->tm_hour : abs((realtime->tm_hour > 12 ? realtime->tm_hour-12 : realtime->tm_hour)),
				CVAR_GET_STRING("cl_clocks_del"), realtime->tm_min,
				CVAR_GET_STRING("cl_clocks_del"), realtime->tm_sec,
				CVAR_GET_FLOAT("cl_clocks_fmt") != 1 ? (realtime->tm_hour < 12 ? "AM" : "PM") : NULL );
			m_flRealTimer = gEngfuncs.GetClientTime()+1;
		}
		sscanf( CVAR_GET_STRING("cl_clocks_pos"), "%f %f", &pTimerPos[0], &pTimerPos[1] );
		pTimerPos[0] *= ScreenWidth;
		pTimerPos[1] *= ScreenHeight;
		pTimerStrLen = 0;
		for( INT i = 0; i < strlen(pRealTimer); i++ )
			pTimerStrLen += gHUD.m_scrinfo.charWidths[ pRealTimer[i] ];
		gHUD.DrawHudString( pTimerPos[0]-(pTimerStrLen/2), pTimerPos[1], ScreenWidth, pRealTimer, pHudColors[0], pHudColors[1], pHudColors[2] );
	}

	if( CVAR_GET_FLOAT("cl_spechud") == 1 && bAmISpec ) {
		if( !CVAR_GET_FLOAT("cl_specbk") ) {
			FillRGBA( 0, 0, ScreenWidth, 64, 0, 0, 0, 255 );
		}
		else{
			sBackgroundSprite = LoadSprite( CVAR_GET_STRING("cl_specbk") );
			SPR_Set( sBackgroundSprite, 255, 255, 255 );
			SPR_Draw( 0, 0, 0, NULL );
		}
		sscanf( CVAR_GET_STRING("cl_specplayers"), "%i %i", &pSpecPlayers[0], &pSpecPlayers[1] );
		if( gEngfuncs.GetEntityByIndex(pSpecPlayers[0]) && gEngfuncs.GetEntityByIndex(pSpecPlayers[1])
			&& gEngfuncs.GetEntityByIndex(pSpecPlayers[0])->curstate.weaponmodel && gEngfuncs.GetEntityByIndex(pSpecPlayers[1])->curstate.weaponmodel ) {
			r = gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][0];
			g = gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][1];
			b = gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][2];
			pTempWeapon = IEngineStudio.GetModelByIndex(gEngfuncs.GetEntityByIndex(pSpecPlayers[0])->curstate.weaponmodel)->name;
			gHUD.DrawHudString( strlen(gHUD.RemoveColors(g_PlayerInfoList[pSpecPlayers[0]].name))*5, 0, ScreenWidth, (char*)gHUD.RemoveColors(g_PlayerInfoList[pSpecPlayers[0]].name), r, g, b );
			gHUD.DrawHudString( strlen(gHUD.RemoveColors(g_PlayerInfoList[pSpecPlayers[0]].name))*5, gHUD.m_iFontHeight, ScreenWidth, (char*)pTempWeapon.substr( 9, pTempWeapon.find_last_of( "." )-9 ).c_str(), r, g, b );
			r = gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][0];
			g = gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][1];
			b = gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][2];
			pTempWeapon = IEngineStudio.GetModelByIndex(gEngfuncs.GetEntityByIndex(pSpecPlayers[1])->curstate.weaponmodel)->name;
			gHUD.DrawHudString( ScreenWidth-(strlen(gHUD.RemoveColors(g_PlayerInfoList[pSpecPlayers[1]].name))*12), 0, ScreenWidth, (char*)gHUD.RemoveColors(g_PlayerInfoList[pSpecPlayers[1]].name), r, g, b );
			gHUD.DrawHudString( ScreenWidth-(strlen(gHUD.RemoveColors(g_PlayerInfoList[pSpecPlayers[1]].name))*12), gHUD.m_iFontHeight, ScreenWidth, (char*)pTempWeapon.substr( 9, pTempWeapon.find_last_of( "." )-9 ).c_str(), r, g, b );
		}
	}
	if( CVAR_GET_FLOAT("cl_spechud") > 1 && g_TeamInfo[1].teamnumber && g_TeamInfo[2].teamnumber ) {
		this->m_iFlags |= HUD_ACTIVE;
		for( INT y = 0; y <= 32; y++ ) {
			if( (g_PlayerExtraInfo[y].teamnumber == g_TeamInfo[1].teamnumber) && !g_PlayerInfoList[y].spectator ) {
				#ifdef _DEBUG
						FillRGBA( 0, 64, 128, gHUD.m_iFontHeight*iStartHeight[0], gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][0],
												gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][1],
												gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][2], 255 );
				#endif
				if( CVAR_GET_FLOAT("cl_spechud") == 3 ) {
					gEngfuncs.pfnDrawSetTextColor( (float)gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][0]/255,
											(float)gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][1]/255,
											(float)gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][2]/255 );
					gEngfuncs.pfnDrawConsoleString( 0, CVAR_GET_FLOAT("cl_spechud_offset")+(gHUD.m_iFontHeight*iStartHeight[0]), (char*)gHUD.RemoveColors(g_PlayerInfoList[y].name) );
				}
				else{
					gHUD.DrawHudString( 0, CVAR_GET_FLOAT("cl_spechud_offset")+(gHUD.m_iFontHeight*iStartHeight[0]), ScreenWidth, (char*)gHUD.RemoveColors(g_PlayerInfoList[y].name), gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][0],
											gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][1],
											gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][2] );
				}
				iStartHeight[0]++;
			}
			else if( g_PlayerExtraInfo[y].teamnumber == g_TeamInfo[2].teamnumber && !g_PlayerInfoList[y].spectator ) {
				#ifdef _DEBUG
				FillRGBA( ScreenWidth-128, 64, 128, gHUD.m_iFontHeight*iStartHeight[1], gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][0],
										gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][1],
										gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][2], 255 );
				#endif
				if( CVAR_GET_FLOAT("cl_spechud") == 3 ) {
					gEngfuncs.pfnDrawSetTextColor( (float)gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][0]/255,
											(float)gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][1]/255,
											(float)gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][2]/255 );
					gEngfuncs.pfnDrawConsoleString( ScreenWidth-strlen(gHUD.RemoveColors(g_PlayerInfoList[y].name))*9, CVAR_GET_FLOAT("cl_spechud_offset")+(gHUD.m_iFontHeight*iStartHeight[1]), (char*)gHUD.RemoveColors(g_PlayerInfoList[y].name) );
				}
				else{
					gHUD.DrawHudString( ScreenWidth-strlen(gHUD.RemoveColors(g_PlayerInfoList[y].name))*9, CVAR_GET_FLOAT("cl_spechud_offset")+(gHUD.m_iFontHeight*iStartHeight[1]), ScreenWidth, (char*)gHUD.RemoveColors(g_PlayerInfoList[y].name), gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][0],
											gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][1],
											gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][2] );
				}
				iStartHeight[1]++;
			}
		}
		iStartHeight[0] = iStartHeight[1] = 1;
		sprintf( pTeamInfo, "%s: %i", g_TeamInfo[1].name, g_TeamInfo[1].frags );
		gHUD.DrawHudString(0, 64, ScreenWidth, pTeamInfo, gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][0],
										gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][1],
										gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][2]);
		sprintf( pTeamInfo, "%s: %i", g_TeamInfo[2].name, g_TeamInfo[2].frags );
		gHUD.DrawHudString(ScreenWidth-strlen(pTeamInfo)*8, 64, ScreenWidth, pTeamInfo, gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][0],
										gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][1],
										gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][2]);
	}

	if( pShouldDrawTimer && CVAR_GET_FLOAT("cl_itemtimer") ) {
		if( bFirst ) {
			iClTime = gEngfuncs.GetClientTime();
			bFirst = FALSE;
		}
		if( gEngfuncs.GetClientTime() < iClTime+pTimerSeconds ) {
			sprintf( pTimerInfo, "Respawn in: %i seconds", (INT)((iClTime+pTimerSeconds)-gEngfuncs.GetClientTime()) );
			if( !(INT)((iClTime+pTimerSeconds)-gEngfuncs.GetClientTime()) ) {
				strcpy( pTimerInfo, "Item spawned!" );
			}
			gEngfuncs.pfnDrawSetTextColor( pHudColors[0]/255, pHudColors[1]/255, pHudColors[2]/255 );
			gEngfuncs.pfnDrawConsoleString( ScreenWidth/2, ScreenHeight/6, pTimerInfo );
		}
		else if( gEngfuncs.GetClientTime() > iClTime+pTimerSeconds ) {
			bFirst = TRUE;
			pShouldDrawTimer = FALSE;
		}
	}

	sscanf(CVAR_GET_STRING("cl_hudcolor"), "%i %i %i %i", &pHudColors[0], &pHudColors[1], &pHudColors[2], &pHudColors[3] );
	if( CVAR_GET_FLOAT("cl_newhud") ) {
		if( !bAmISpec && !gEngfuncs.GetLocalPlayer()->curstate.spectator ) {
			if( CVAR_GET_FLOAT("cl_drawmyname") )
				gHUD.DrawHudString( (ScreenWidth/2)-(4*strlen( gHUD.RemoveColors(CVAR_GET_STRING("name")) )), ScreenHeight-gHUD.m_scrinfo.iCharHeight-8, ScreenWidth, (char*)gHUD.RemoveColors(CVAR_GET_STRING("name")), 255-(m_iHealth*2.55), m_iHealth*2.55, 0 );
			gHUD.DrawHudNumber( 24, ScreenHeight-gHUD.m_scrinfo.iCharHeight-8, DHN_3DIGITS | DHN_DRAWZERO, m_iHealth, pHudColors[0], pHudColors[1], pHudColors[2] );
			if( gHUD.m_Teamplay ) {
				sprintf( pTeamInfo, "Your team: %s", g_PlayerExtraInfo[gEngfuncs.GetLocalPlayer()->index].teamname );
				gHUD.DrawHudString( (ScreenWidth/2)-(4*strlen( pTeamInfo )), 0, ScreenWidth, pTeamInfo, pHudColors[0], pHudColors[1], pHudColors[2] );
			}
		}
	}

	if( CVAR_GET_FLOAT("cl_drawteams") && CVAR_GET_FLOAT("cl_spechud") < 2 && gHUD.m_Teamplay ) {
		FillRGBA(0, 0, 128, 20, gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][0],
										gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][1],
										gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][2], 150);
		FillRGBA(0, 20, 128, 20, gTeamColors[ g_TeamInfo[1].teamnumber % gNumberOfTeamColors ][0],
										gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][1],
										gTeamColors[ g_TeamInfo[2].teamnumber % gNumberOfTeamColors ][2], 150);
		gHUD.DrawHudString(0, 0, 128, g_TeamInfo[1].name, 255, 255, 255);
		gHUD.DrawHudString(0, 18, 128, g_TeamInfo[2].name, 255, 255, 255);
		if( CVAR_GET_FLOAT("cl_drawteamscores") ) {
			sprintf( pFrags, "%i", g_TeamInfo[1].frags );
			gHUD.DrawHudString(100, 0, ScreenWidth, pFrags, 255, 255, 255 );
			sprintf( pFrags, "%i", g_TeamInfo[2].frags );
			gHUD.DrawHudString(100, 20, ScreenWidth, pFrags, 255, 255, 255 );
		}
	}

	sscanf( CVAR_GET_STRING("cl_crosscolor"), "%i %i %i", &pCrossColors[0], &pCrossColors[1], &pCrossColors[2] );
	if( CVAR_GET_FLOAT("cl_smart_crosshair") && bCrosshairMustBeRed ) {
		sscanf( CVAR_GET_STRING("cl_smartcrosscolor"), "%i %i %i", &pCrossColors[0], &pCrossColors[1], &pCrossColors[2] );
	}
	switch( (INT)CVAR_GET_FLOAT("cl_crosshair") ) {
		case 1:
			gEngfuncs.pfnFillRGBA( (ScreenWidth/2)-(CVAR_GET_FLOAT("cl_crossdotsize")/2), (ScreenHeight/2)-(CVAR_GET_FLOAT("cl_crossdotsize")/2), CVAR_GET_FLOAT("cl_crossdotsize"), CVAR_GET_FLOAT("cl_crossdotsize"), pCrossColors[0], pCrossColors[1], pCrossColors[2], 255 );
		break;
		case 2:
			gEngfuncs.pfnFillRGBA( (ScreenWidth/2)-4, (ScreenHeight/2), 10, 2, pCrossColors[0], pCrossColors[1], pCrossColors[2], 255 );
			gEngfuncs.pfnFillRGBA( (ScreenWidth/2), (ScreenHeight/2)-4, 2, 10, pCrossColors[0], pCrossColors[1], pCrossColors[2], 255 );
		break;
		case 3:
			gEngfuncs.pfnFillRGBA( (ScreenWidth/2)-2, (ScreenHeight/2)-2, 4, 4, pCrossColors[0], pCrossColors[1], pCrossColors[2], 255 );
			gEngfuncs.pfnFillRGBA( (ScreenWidth/2)-3, (ScreenHeight/2)-3, 6, 6, 0, 255, 0, 255 );
		break;
		case 4:
			gEngfuncs.pfnFillRGBA( (ScreenWidth/2)-10, (ScreenHeight/2)-2, 10, 2, pCrossColors[0], pCrossColors[1], pCrossColors[2], 255 );
			gEngfuncs.pfnFillRGBA( (ScreenWidth/2)-2, (ScreenHeight/2)-10, 2, 10, pCrossColors[0], pCrossColors[1], pCrossColors[2], 255 );
		break;
		case 5:
			char szCrosshairSprite[64];
			sprintf( szCrosshairSprite, "sprites/%s.spr", CVAR_GET_STRING("cl_cross_sprname") );
			SPR_Set( LoadSprite(szCrosshairSprite), pCrossColors[0], pCrossColors[1], pCrossColors[2] );
			SPR_DrawAdditive( 0, (ScreenWidth/2)-(CVAR_GET_FLOAT("cl_cross_sprsize")/2), (ScreenHeight/2)-(CVAR_GET_FLOAT("cl_cross_sprsize")/2), NULL );
		break;
	}

	if( bShouldShowScoreboard ) {
		char szPlayerInfo[64];
		int r, g, b;
		int score_r, score_g, score_b, score_a;
		int playeroffset = 16;
		sscanf(CVAR_GET_STRING("cl_hudcolor"), "%i %i %i", &r, &g, &b );
		sscanf(CVAR_GET_STRING("cl_scorecolor"), "%i %i %i", &score_r, &score_g, &score_b, &score_a );
		gHUD.DrawHudString( (ScreenWidth/2)-strlen( gViewPort->m_szServerName )*4+CVAR_GET_FLOAT("cl_newscore_offset_x"), 16+CVAR_GET_FLOAT("cl_newscore_offset_y"), ScreenWidth, gViewPort->m_szServerName, 255, 255, 255 );
		FillRGBA( 0, 36+CVAR_GET_FLOAT("cl_newscore_offset_y"), ScreenWidth, ScreenHeight-36, score_r, score_g, score_b, score_a );
		for( int i = 1; i < MAX_PLAYERS; i++ ) {
			if( g_PlayerInfoList[i].name != NULL ) {
				playeroffset += 18;
				sscanf(CVAR_GET_STRING("cl_hudcolor"), "%i %i %i", &r, &g, &b );
				if( gHUD.m_Teamplay ) {
					r = pTeamColors[g_PlayerExtraInfo[i].teamnumber % gNumberOfTeamColors][0];
					g = pTeamColors[g_PlayerExtraInfo[i].teamnumber % gNumberOfTeamColors][1];
					b = pTeamColors[g_PlayerExtraInfo[i].teamnumber % gNumberOfTeamColors][2];
				}
				if( g_PlayerInfoList[i].thisplayer )
					r = g = b = 255; //highlight myself
				sprintf( szPlayerInfo, "%s    %s    %i / %i (%i)", g_PlayerExtraInfo[i].teamname, g_PlayerInfoList[i].name, g_PlayerExtraInfo[i].frags, g_PlayerExtraInfo[i].deaths, g_PlayerInfoList[i].ping );
				gHUD.DrawHudString( (ScreenWidth/2)-strlen(szPlayerInfo)*4+CVAR_GET_FLOAT("cl_newscore_offset_x"), playeroffset+CVAR_GET_FLOAT("cl_newscore_offset_y"), ScreenWidth, szPlayerInfo, r, g, b );
			}
		}
	}

	//if ( (gHUD.m_iHideHUDDisplay & HIDEHUD_HEALTH) || gEngfuncs.IsSpectateOnly() )
	//	return 1;
		if ( !m_SpriteHandle_t )
			m_SpriteHandle_t = LoadSprite(PAIN_NAME);
	if( !CVAR_GET_FLOAT("cl_newhud") ) {
		// Has health changed? Flash the health #
		/*if (m_fFade)
		{
			m_fFade -= (gHUD.m_flTimeDelta * 20);
			if (m_fFade <= 0)
			{
				a = MIN_ALPHA;
				m_fFade = 0;
			}

			// Fade the health number back to dim

			a = MIN_ALPHA +  (m_fFade/FADE_TIME) * 128;

		}
		else
			a = MIN_ALPHA;*/

		// If health is getting low, make it bright red
		//if (m_iHealth <= 15)
		//	a = 255;
		
		GetPainColor( r, g, b );
		ScaleColors(r, g, b, a );

		// Only draw health if we have the suit.
		if (gHUD.m_iWeaponBits & (1<<(WEAPON_SUIT)))
		{
			HealthWidth = gHUD.GetSpriteRect(gHUD.m_HUD_number_0).right - gHUD.GetSpriteRect(gHUD.m_HUD_number_0).left;
			int CrossWidth = gHUD.GetSpriteRect(m_HUD_cross).right - gHUD.GetSpriteRect(m_HUD_cross).left;

			y = ScreenHeight - gHUD.m_iFontHeight - gHUD.m_iFontHeight / 2;
			x = CrossWidth /2;

			SPR_Set(gHUD.GetSprite(m_HUD_cross), pHudColors[0], pHudColors[1], pHudColors[2]);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(m_HUD_cross));

			x = CrossWidth + HealthWidth / 2;

			x = gHUD.DrawHudNumber(x, y, DHN_3DIGITS | DHN_DRAWZERO, m_iHealth, pHudColors[0], pHudColors[1], pHudColors[2]);

			x += HealthWidth/2;

			int iHeight = gHUD.m_iFontHeight;
			int iWidth = HealthWidth/10;
			FillRGBA(x, y, iWidth, iHeight, pHudColors[0], pHudColors[1], pHudColors[2], pHudColors[3]);
		}
	}
	DrawDamage(flTime);
	return DrawPain(flTime);
}

void CHudHealth::CalcDamageDirection(vec3_t vecFrom)
{
	vec3_t	forward, right, up;
	float	side, front;
	vec3_t vecOrigin, vecAngles;

	if (!vecFrom[0] && !vecFrom[1] && !vecFrom[2])
	{
		m_fAttackFront = m_fAttackRear = m_fAttackRight = m_fAttackLeft = 0;
		return;
	}


	memcpy(vecOrigin, gHUD.m_vecOrigin, sizeof(vec3_t));
	memcpy(vecAngles, gHUD.m_vecAngles, sizeof(vec3_t));


	VectorSubtract (vecFrom, vecOrigin, vecFrom);

	float flDistToTarget = vecFrom.Length();

	vecFrom = vecFrom.Normalize();
	AngleVectors (vecAngles, forward, right, up);

	front = DotProduct (vecFrom, right);
	side = DotProduct (vecFrom, forward);

	if (flDistToTarget <= 50)
	{
		m_fAttackFront = m_fAttackRear = m_fAttackRight = m_fAttackLeft = 1;
	}
	else 
	{
		if (side > 0)
		{
			if (side > 0.3)
				m_fAttackFront = max(m_fAttackFront, side);
		}
		else
		{
			float f = fabs(side);
			if (f > 0.3)
				m_fAttackRear = max(m_fAttackRear, f);
		}

		if (front > 0)
		{
			if (front > 0.3)
				m_fAttackRight = max(m_fAttackRight, front);
		}
		else
		{
			float f = fabs(front);
			if (f > 0.3)
				m_fAttackLeft = max(m_fAttackLeft, f);
		}
	}
}

int CHudHealth::DrawPain(float flTime)
{
	if (!(m_fAttackFront || m_fAttackRear || m_fAttackLeft || m_fAttackRight))
		return 1;

	if( CVAR_GET_FLOAT("cl_damagepunch") ) {
		if( m_fAttackLeft )
			V_PunchAxis( 2, 10 );
		else if( m_fAttackRight )
			V_PunchAxis( 2, -10 );
		else
			V_PunchAxis( 3, 10 );
	}

	int r, g, b;
	int x, y, a, shade;

	// TODO:  get the shift value of the health
	a = 255;	// max brightness until then

	float fFade = gHUD.m_flTimeDelta * 2;
	
	// SPR_Draw top
	if (m_fAttackFront > 0.4)
	{
		GetPainColor(r,g,b);
		shade = a * max( m_fAttackFront, 0.5 );
		ScaleColors(r, g, b, shade);
		SPR_Set(m_SpriteHandle_t, r, g, b );

		x = ScreenWidth/2 - SPR_Width(m_SpriteHandle_t, 0)/2;
		y = ScreenHeight/2 - SPR_Height(m_SpriteHandle_t,0) * 3;
		SPR_DrawAdditive(0, x, y, NULL);
		m_fAttackFront = max( 0, m_fAttackFront - fFade );
	} else
		m_fAttackFront = 0;

	if (m_fAttackRight > 0.4)
	{
		GetPainColor(r,g,b);
		shade = a * max( m_fAttackRight, 0.5 );
		ScaleColors(r, g, b, shade);
		SPR_Set(m_SpriteHandle_t, r, g, b );

		x = ScreenWidth/2 + SPR_Width(m_SpriteHandle_t, 1) * 2;
		y = ScreenHeight/2 - SPR_Height(m_SpriteHandle_t,1)/2;
		SPR_DrawAdditive(1, x, y, NULL);
		m_fAttackRight = max( 0, m_fAttackRight - fFade );
	} else
		m_fAttackRight = 0;

	if (m_fAttackRear > 0.4)
	{
		GetPainColor(r,g,b);
		shade = a * max( m_fAttackRear, 0.5 );
		ScaleColors(r, g, b, shade);
		SPR_Set(m_SpriteHandle_t, r, g, b );

		x = ScreenWidth/2 - SPR_Width(m_SpriteHandle_t, 2)/2;
		y = ScreenHeight/2 + SPR_Height(m_SpriteHandle_t,2) * 2;
		SPR_DrawAdditive(2, x, y, NULL);
		m_fAttackRear = max( 0, m_fAttackRear - fFade );
	} else
		m_fAttackRear = 0;

	if (m_fAttackLeft > 0.4)
	{
		GetPainColor(r,g,b);
		shade = a * max( m_fAttackLeft, 0.5 );
		ScaleColors(r, g, b, shade);
		SPR_Set(m_SpriteHandle_t, r, g, b );

		x = ScreenWidth/2 - SPR_Width(m_SpriteHandle_t, 3) * 3;
		y = ScreenHeight/2 - SPR_Height(m_SpriteHandle_t,3)/2;
		SPR_DrawAdditive(3, x, y, NULL);

		m_fAttackLeft = max( 0, m_fAttackLeft - fFade );
	} else
		m_fAttackLeft = 0;

	return 1;
}

int CHudHealth::DrawDamage(float flTime)
{
	int r, g, b, a;
	DAMAGE_IMAGE *pdmg;

	if (!m_bitsDamage)
		return 1;

	UnpackRGB(r,g,b, RGB_YELLOWISH);
	
	a = (int)( fabs(sin(flTime*2)) * 256.0);

	ScaleColors(r, g, b, a);

	// Draw all the items
	int i;
	for (i = 0; i < NUM_DMG_TYPES; i++)
	{
		if (m_bitsDamage & giDmgFlags[i])
		{
			pdmg = &m_dmg[i];
			SPR_Set(gHUD.GetSprite(m_HUD_dmg_bio + i), r, g, b );
			SPR_DrawAdditive(0, pdmg->x, pdmg->y, &gHUD.GetSpriteRect(m_HUD_dmg_bio + i));
		}
	}


	// check for bits that should be expired
	for ( i = 0; i < NUM_DMG_TYPES; i++ )
	{
		DAMAGE_IMAGE *pdmg = &m_dmg[i];

		if ( m_bitsDamage & giDmgFlags[i] )
		{
			pdmg->fExpire = min( flTime + DMG_IMAGE_LIFE, pdmg->fExpire );

			if ( pdmg->fExpire <= flTime		// when the time has expired
				&& a < 40 )						// and the flash is at the low point of the cycle
			{
				pdmg->fExpire = 0;

				int y = pdmg->y;
				pdmg->x = pdmg->y = 0;

				// move everyone above down
				for (int j = 0; j < NUM_DMG_TYPES; j++)
				{
					pdmg = &m_dmg[j];
					if ((pdmg->y) && (pdmg->y < y))
						pdmg->y += giDmgHeight;

				}

				m_bitsDamage &= ~giDmgFlags[i];  // clear the bits
			}
		}
	}

	return 1;
}
 

void CHudHealth::UpdateTiles(float flTime, long bitsDamage)
{	
	DAMAGE_IMAGE *pdmg;

	// Which types are new?
	long bitsOn = ~m_bitsDamage & bitsDamage;
	
	for (int i = 0; i < NUM_DMG_TYPES; i++)
	{
		pdmg = &m_dmg[i];

		// Is this one already on?
		if (m_bitsDamage & giDmgFlags[i])
		{
			pdmg->fExpire = flTime + DMG_IMAGE_LIFE; // extend the duration
			if (!pdmg->fBaseline)
				pdmg->fBaseline = flTime;
		}

		// Are we just turning it on?
		if (bitsOn & giDmgFlags[i])
		{
			// put this one at the bottom
			pdmg->x = giDmgWidth/8;
			pdmg->y = ScreenHeight - giDmgHeight * 2;
			pdmg->fExpire=flTime + DMG_IMAGE_LIFE;
			
			// move everyone else up
			for (int j = 0; j < NUM_DMG_TYPES; j++)
			{
				if (j == i)
					continue;

				pdmg = &m_dmg[j];
				if (pdmg->y)
					pdmg->y -= giDmgHeight;

			}
			pdmg = &m_dmg[i];
		}	
	}	

	// damage bits are only turned on here;  they are turned off when the draw time has expired (in DrawDamage())
	m_bitsDamage |= bitsDamage;
}
