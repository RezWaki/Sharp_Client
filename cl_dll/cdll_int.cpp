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
//  cdll_int.c
//
// this implementation handles the linking of the engine to the DLL
//

#include "netadr.h"
#include "vgui_schememanager.h"
#include "sharpfuncs.h"

extern "C"
{
#include "pm_shared.h"
}

#include <string.h>
#include "hud_servers.h"
#include "vgui_int.h"
#include "interface.h"
#ifdef _DEBUG
#include <GL\glew.h>
#endif

#define DLLEXPORT __declspec( dllexport )


cl_enginefunc_t gEngfuncs;
CHud gHUD;
TeamFortressViewport *gViewPort = NULL;
extern CFonts gpFonts;
extern cvar_t* g_phud_timer;
extern cvar_t* g_pcl_liveupdate;

void InitInput (void);
void EV_HookEvents( void );
void IN_Commands( void );

/*
========================== 
    Initialize

Called when the DLL is first loaded.
==========================
*/
extern "C" 
{
int		DLLEXPORT Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion );
int		DLLEXPORT HUD_VidInit( void );
void	DLLEXPORT HUD_Init( void );
int		DLLEXPORT HUD_Redraw( float flTime, int intermission );
int		DLLEXPORT HUD_UpdateClientData( client_data_t *cdata, float flTime );
void	DLLEXPORT HUD_Reset ( void );
void	DLLEXPORT HUD_PlayerMove( struct playermove_s *ppmove, int server );
void	DLLEXPORT HUD_PlayerMoveInit( struct playermove_s *ppmove );
char	DLLEXPORT HUD_PlayerMoveTexture( char *name );
int		DLLEXPORT HUD_ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size );
int		DLLEXPORT HUD_GetHullBounds( int hullnumber, float *mins, float *maxs );
void	DLLEXPORT HUD_Frame( double time );
void	DLLEXPORT HUD_VoiceStatus(int entindex, qboolean bTalking);
void	DLLEXPORT HUD_DirectorMessage( int iSize, void *pbuf );
}

/*
================================
HUD_GetHullBounds

  Engine calls this to enumerate player collision hulls, for prediction.  Return 0 if the hullnumber doesn't exist.
================================
*/
int DLLEXPORT HUD_GetHullBounds( int hullnumber, float *mins, float *maxs )
{
	int iret = 0;

	switch ( hullnumber )
	{
	case 0:				// Normal player
		mins = Vector(-16, -16, -36);
		maxs = Vector(16, 16, 36);
		iret = 1;
		break;
	case 1:				// Crouched player
		mins = Vector(-16, -16, -18 );
		maxs = Vector(16, 16, 18 );
		iret = 1;
		break;
	case 2:				// Point based hull
		mins = Vector( 0, 0, 0 );
		maxs = Vector( 0, 0, 0 );
		iret = 1;
		break;
	}

	return iret;
}

/*
================================
HUD_ConnectionlessPacket

 Return 1 if the packet is valid.  Set response_buffer_size if you want to send a response packet.  Incoming, it holds the max
  size of the response_buffer, so you must zero it out if you choose not to respond.
================================
*/
int	DLLEXPORT HUD_ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size )
{
	// Parse stuff from args
	int max_buffer_size = *response_buffer_size;

	// Zero it out since we aren't going to respond.
	// If we wanted to response, we'd write data into response_buffer
	*response_buffer_size = 0;

	// Since we don't listen for anything here, just respond that it's a bogus message
	// If we didn't reject the message, we'd return 1 for success instead.
	return 0;
}

void DLLEXPORT HUD_PlayerMoveInit( struct playermove_s *ppmove )
{
	PM_Init( ppmove );
}

char DLLEXPORT HUD_PlayerMoveTexture( char *name )
{
	return PM_FindTextureType( name );
}

void DLLEXPORT HUD_PlayerMove( struct playermove_s *ppmove, int server )
{
	PM_Move( ppmove, server );
}

char pClVersionInfo[256];

int DLLEXPORT Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion )
{
	gEngfuncs = *pEnginefuncs;

	if (iVersion != CLDLL_INTERFACE_VERSION)
		return 0;

	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	//NULL flag -> FCVAR_ARCHIVE
	CVAR_CREATE( "cl_scorecolor", "0 15 20 150", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_scorehighlight", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_hudcolorforname", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_hudcolor", "255 0 0 255", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_vote_defaultpos", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_gaussbeam_color", "150 0 255", FCVAR_ARCHIVE );
	CVAR_CREATE( "r_fakedrawentities", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_hudweapon", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_smart_crosshair", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "r_noplayerlights", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "r_noitemlights", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "r_forcerendercolors", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_newhud", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_usenewteamcolors", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_hltvmode", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "r_extrachrome", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_flashplayer", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_specwh", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_itemtimer", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_rainbowhud", "0", FCVAR_ARCHIVE );
	//CVAR_CREATE( "cl_models_subfolder", "1", FCVAR_ARCHIVE );
	//CVAR_CREATE( "cl_restrictspecs", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_drawteams", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_drawteamscores", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_crosshair", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_crosscolor", "255 255 255", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_crossdotsize", "4", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_spechud", "0", FCVAR_ARCHIVE );
	//CVAR_CREATE( "cl_specbk", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_specplayers", "1 2", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_specteams", "1 2", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_scorepanel_offsets", "0 0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_spechud_offset", "128", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_damagepunch", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_disabledynamiclights", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_forcebeamcolors", "255 0 0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_blockclientcmd", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_smartcrosscolor", "255 0 0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_newscoreboard", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_newscore_offset_x", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_newscore_offset_y", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_cross_sprname", "mycrosshair", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_cross_sprsize", "32", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_weaponpos", "0 0 0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_oldladdersteps", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_specoffset", "28", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_traceline", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_maxtracelines", "4", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_traceline_color", "255 0 0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_cross_trace", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_autorecord", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_autobhop", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_slide", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_slide_interval", "0.05", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_disablespecs", "0", FCVAR_ARCHIVE ); //ag 6.6
	CVAR_CREATE( "cl_usenewhudstring", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_drawmyname", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_transparent_wpnmodels", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_hidehudsininterm", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_showscoreboardinintermission", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_showinterstats", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_blackdeathscreen", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_deathcam_height", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_deathcam_angle", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_clocks", "0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_clocks_pos", "0.5 0", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_clocks_fmt", "1", FCVAR_ARCHIVE );
	CVAR_CREATE( "cl_clocks_del", ":", FCVAR_ARCHIVE );
	g_phud_timer = gEngfuncs.pfnRegisterVariable( "hud_timer", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	g_pcl_liveupdate = gEngfuncs.pfnRegisterVariable( "cl_liveupdate", "0", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
#ifdef _DEBUG
	CVAR_CREATE( "r_glwireframecolors", "255 0 0 255", NULL );
	CVAR_CREATE( "r_glwireframewidth", "4", NULL );
	CVAR_CREATE( "r_glwireframe", "0", NULL );
	glewInit();
#endif

	sprintf( pClVersionInfo, "echo Sharp Client Version [%s]", CL_VER );
	gEngfuncs.pfnClientCmd( "toggleconsole" );
	gEngfuncs.pfnClientCmd( "clear" );
	gEngfuncs.pfnClientCmd( pClVersionInfo );
	gEngfuncs.pfnClientCmd( "echo \"Web-site: http://rezwaki.hldns.ru/sharp/\"" );
	gEngfuncs.pfnClientCmd( "echo \"GitHub page: https://github.com/RezWaki/Sharp_Client/\"" );

	EV_HookEvents();

	gpFonts.Init();

	return 1;
}


/*
==========================
	HUD_VidInit

Called when the game initializes
and whenever the vid_mode is changed
so the HUD can reinitialize itself.
==========================
*/

int DLLEXPORT HUD_VidInit( void )
{
	gHUD.VidInit();

	VGui_Startup();

	return 1;
}

/*
==========================
	HUD_Init

Called whenever the client connects
to a server.  Reinitializes all 
the hud variables.
==========================
*/

void DLLEXPORT HUD_Init( void )
{
	InitInput();
	gHUD.Init();
	Scheme_Init();
}


/*
==========================
	HUD_Redraw

called every screen frame to
redraw the HUD.
===========================
*/

int DLLEXPORT HUD_Redraw( float time, int intermission )
{
	gHUD.Redraw( time, intermission );

	return 1;
}


/*
==========================
	HUD_UpdateClientData

called every time shared client
dll/engine data gets changed,
and gives the cdll a chance
to modify the data.

returns 1 if anything has been changed, 0 otherwise.
==========================
*/

int DLLEXPORT HUD_UpdateClientData(client_data_t *pcldata, float flTime )
{
	IN_Commands();

	return gHUD.UpdateClientData(pcldata, flTime );
}

/*
==========================
	HUD_Reset

Called at start and end of demos to restore to "non"HUD state.
==========================
*/

void DLLEXPORT HUD_Reset( void )
{
	gHUD.VidInit();
}

/*
==========================
HUD_Frame

Called by engine every frame that client .dll is loaded
==========================
*/

void DLLEXPORT HUD_Frame( double time )
{
	ServersThink( time );

	GetClientVoiceMgr()->Frame(time);
}


/*
==========================
HUD_VoiceStatus

Called when a player starts or stops talking.
==========================
*/

void DLLEXPORT HUD_VoiceStatus(int entindex, qboolean bTalking)
{
	GetClientVoiceMgr()->UpdateSpeakerStatus(entindex, bTalking);
}

/*
==========================
HUD_DirectorEvent

Called when a director event message was received
==========================
*/

void DLLEXPORT HUD_DirectorMessage( int iSize, void *pbuf )
{
	 gHUD.m_Spectator.DirectorMessage( iSize, pbuf );
}


