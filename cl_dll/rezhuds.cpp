#include "sharpfuncs.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_ScorePanel.h"
#include <ctime>
extern INT gTeamColors[5][3];
extern INT pTeamColors[5][3];;
extern INT gNumberOfTeamColors;
extern BOOL bIsReloading, bCrosshairMustBeRed;
extern extra_player_info_t g_PlayerExtraInfo[MAX_PLAYERS+1];
extern engine_studio_api_t IEngineStudio;

INT CHudSpeedometer::Init( void ) {
	pPlayerSpeed = 0;
	m_iFlags = HUD_ACTIVE;
	gHUD.AddHudElem(this);
	return 1;
}

INT CHudSpeedometer::Draw( float flTime ) {
	switch( (INT)CVAR_GET_FLOAT("cl_speedometer") ) {
		case 1:
			sprintf( pSpeed, "%03i", pPlayerSpeed );
			gHUD.DrawHudString( ScreenWidth/2-gHUD.HudStringWidth(pSpeed)/2, ScreenHeight-gHUD.m_iFontHeight-gHUD.m_iFontHeight/2, ScreenWidth, pSpeed, gHUD.pHudColors[0], gHUD.pHudColors[1], gHUD.pHudColors[2] );
		break;
		case 2:
			gHUD.DrawHudNumber( ScreenWidth/2, ScreenHeight-gHUD.m_iFontHeight-gHUD.m_iFontHeight/2, HUD_ACTIVE, pPlayerSpeed, gHUD.pHudColors[0], gHUD.pHudColors[1], gHUD.pHudColors[2] );
		break;
		case 3:
			sprintf( pSpeed, "%03i", pPlayerSpeed );
			gEngfuncs.pfnDrawSetTextColor( gHUD.pHudColors[0]/255, gHUD.pHudColors[1]/255, gHUD.pHudColors[2]/255 );
			gEngfuncs.pfnDrawConsoleStringLen( pSpeed, &width, &height );
			gEngfuncs.pfnDrawConsoleString( ScreenWidth/2-(width/2), ScreenHeight-gHUD.m_iFontHeight-gHUD.m_iFontHeight/2, pSpeed );
		break;
	}
	return 1;
}

INT CHudClocks::Init( void ) {
	pTimerStrLen = 0; m_flRealTimer = 0;
	pTimerPos[0] = 0; pTimerPos[1] = 0;
	m_iFlags = HUD_ACTIVE;
	gHUD.AddHudElem(this);
	return 1;
}

INT CHudClocks::Draw( float flTime ) {
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
		gHUD.DrawHudString( pTimerPos[0]-(pTimerStrLen/2), pTimerPos[1], ScreenWidth, pRealTimer, gHUD.pHudColors[0], gHUD.pHudColors[1], gHUD.pHudColors[2] );
	}
	return 1;
}

INT CItemSpawnTimer::Init( void ) {
	for( INT i = 0; i < MAX_IT_ITEMS; i++ ) {
		bFirst[i] = TRUE;
		pTimerSeconds[i] = -1;
		pShouldDrawTimer[i] = FALSE;
		iClTime[i] = 0;
	}
	y_pos = width = height = 0;
	m_iFlags = HUD_ACTIVE;
	gHUD.AddHudElem(this);
	return 1;
}

INT CItemSpawnTimer::Draw( float flTime ) {
	if( !CVAR_GET_FLOAT("cl_istimer") ) return 1;
	for( INT i = 0; i < CVAR_GET_FLOAT("cl_istimer"); i++ ) {
		if( CVAR_GET_FLOAT("cl_istimer_pos") ) {
			sscanf( CVAR_GET_STRING("cl_istimer_pos"), "%f %f", &pos[0], &pos[1] );
			pos[0] *= ScreenWidth; pos[1] *= ScreenHeight;
		}
		sscanf( CVAR_GET_STRING("cl_istimer_offsets"), "%i %i", &offsets[0], &offsets[1] );
		if( pShouldDrawTimer[i] ) {
			if( bFirst[i] ) {
				iClTime[i] = gEngfuncs.GetClientTime();
				if( !isdigit(pItemNames[i][0]) ) pItemNames[i][0] -= 32;
				bFirst[i] = FALSE;
			}
			if( pTimerSeconds[i] != -1 && gEngfuncs.GetClientTime() < iClTime[i]+pTimerSeconds[i] ) {
				sprintf( pTimerInfo, CVAR_GET_STRING("cl_istimer_fmt"), pItemNames[i], (INT)((iClTime[i]+pTimerSeconds[i])-gEngfuncs.GetClientTime()) );
				if( !(INT)((iClTime[i]+pTimerSeconds[i])-gEngfuncs.GetClientTime()) ) {
					pTimerSeconds[i] = -1;
					pShouldDrawTimer[i] = FALSE;
				}
				gEngfuncs.pfnDrawSetTextColor( gHUD.pHudColors[0]/255, gHUD.pHudColors[1]/255, gHUD.pHudColors[2]/255 );
				gEngfuncs.pfnDrawConsoleStringLen( pTimerInfo, &width, &height );
				gEngfuncs.pfnDrawConsoleString( (CVAR_GET_FLOAT("cl_istimer_pos") ? pos[0] : ScreenWidth/2-width/2+offsets[0]),
					(CVAR_GET_FLOAT("cl_istimer_pos") ? (pos[1]+height*i) : (ScreenHeight/2+height*i+offsets[1])),
					pTimerInfo );
			}
			else if( gEngfuncs.GetClientTime() > iClTime[i]+pTimerSeconds[i] ) {
				bFirst[i] = TRUE;
				pShouldDrawTimer[i] = FALSE;
			}
		}
	}
	return 1;
}

INT CHudTeamsBoard::Init( void ) {
	y_offset = title_y_offset = 0;
	m_iFlags = HUD_ACTIVE;
	gHUD.AddHudElem(this);
	return 1;
}

INT CHudTeamsBoard::Draw( float flTime ) {
	if( CVAR_GET_FLOAT("cl_drawteamsboard") && CVAR_GET_FLOAT("cl_spechud") < 2 && gHUD.m_Teamplay ) {
		sscanf( CVAR_GET_STRING("cl_teamsboard_pos"), "%f %f", &pos[0], &pos[1] );
		sscanf( CVAR_GET_STRING("cl_teamsboard_fntclr"), "%i %i %i", &font_color[0], &font_color[1], &font_color[2] );
		pos[0] *= ScreenWidth; pos[1] *= ScreenHeight;
		for( INT i = 0; i < gViewPort->GetScoreBoard()->m_iNumTeams; i++ ) {
			FillRGBA( 0+pos[0], y_offset+pos[1], 128, 20, pTeamColors[ g_TeamInfo[i+1].teamnumber % gNumberOfTeamColors ][0],
											pTeamColors[ g_TeamInfo[i+1].teamnumber % gNumberOfTeamColors ][1],
											pTeamColors[ g_TeamInfo[i+1].teamnumber % gNumberOfTeamColors ][2], CVAR_GET_FLOAT("cl_teamsboard_alpha") );
			gHUD.DrawHudString(0+pos[0], title_y_offset+pos[1], ScreenWidth, g_TeamInfo[i+1].name, font_color[0], font_color[1], font_color[2] );
			if( CVAR_GET_FLOAT("cl_drawteamscores") ) {
				sprintf( pFrags, "%i", g_TeamInfo[i+1].frags );
				gHUD.DrawHudString(100+pos[0], y_offset+pos[1], ScreenWidth, pFrags, font_color[0], font_color[1], font_color[2] );
			}
			y_offset += 20;
			title_y_offset += 18;
		}
		y_offset = title_y_offset = 0;
	}
	return 1;
}

INT CCustomCrosshair::Init( void ) {
	bCrosshairMustBeRed = FALSE;
	bMustBeDrawn = TRUE;
	m_iFlags = HUD_ACTIVE;
	gHUD.AddHudElem( this );
	return 1;
}

INT CCustomCrosshair::Draw( float flTime ) {
	if( CVAR_GET_FLOAT("cl_crosshair") ) {
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
				sprintf( szCrosshairSprite, "sprites/%s.spr", CVAR_GET_STRING("cl_cross_sprname") );
				SPR_Set( LoadSprite(szCrosshairSprite), pCrossColors[0], pCrossColors[1], pCrossColors[2] );
				SPR_DrawAdditive( 0, (ScreenWidth/2)-(CVAR_GET_FLOAT("cl_cross_sprsize")/2), (ScreenHeight/2)-(CVAR_GET_FLOAT("cl_cross_sprsize")/2), NULL );
			break;
		}
	}
	return 1;
}

INT CHudReloadStatus::Init( void ) {
	bIsReloading = FALSE;
	m_iFlags = HUD_ACTIVE;
	gHUD.AddHudElem( this );
	return 1;
}

INT CHudReloadStatus::Draw( float flTime ) {
	if( CVAR_GET_FLOAT("r_drawviewmodel") || !CVAR_GET_FLOAT("cl_reloadstatus") ) return 1;
	if( bIsReloading ) {
		sscanf( CVAR_GET_STRING("cl_reloadstatus_pos"), "%f %f", &pReloadStatusPos[0], &pReloadStatusPos[1] );
		gHUD.DrawHudString( pReloadStatusPos[0]*ScreenWidth-(gHUD.HudStringWidth("Reloading...")/2), pReloadStatusPos[1]*ScreenHeight, ScreenWidth, "Reloading...", gHUD.pHudColors[0], gHUD.pHudColors[1], gHUD.pHudColors[2] );
	}
	return 1;
}

INT CSpectatorHud::Init( void ) {
	m_iFlags = HUD_ACTIVE;
	gHUD.AddHudElem( this );
	iStartHeight[0] = iStartHeight[1] = 1;
	return 1;
}

INT CSpectatorHud::Draw( float flTime ) {
	return 1;
}