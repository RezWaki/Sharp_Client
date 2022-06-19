//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// Triangle rendering, if any

#include "sharpfuncs.h"

// Triangle rendering apis are in gEngfuncs.pTriAPI

extern CRenderFuncs gpRenderFuncs;

#define MAX_TRACELINES 100

#define DLLEXPORT __declspec( dllexport )

extern "C"
{
	void DLLEXPORT HUD_DrawNormalTriangles( void );
	void DLLEXPORT HUD_DrawTransparentTriangles( void );
};

extern engine_studio_api_t IEngineStudio;

INT iTexId = 0, bFound = FALSE;

//#define TEST_IT
#if defined( TEST_IT )

/*
=================
Draw_Triangles

Example routine.  Draws a sprite offset from the player origin.
=================
*/
void Draw_Triangles( void )
{
	cl_entity_t *player;
	vec3_t org;

	// Load it up with some bogus data
	player = gEngfuncs.GetLocalPlayer();
	if ( !player )
		return;

	org = player->origin;

	org.x += 50;
	org.y += 50;

	if (gHUD.m_hsprCursor == 0)
	{
		char sz[256];
		sprintf( sz, "sprites/cursor.spr" );
		gHUD.m_hsprCursor = SPR_Load( sz );
	}

	if ( !gEngfuncs.pTriAPI->SpriteTexture( (struct model_s *)gEngfuncs.GetSpritePointer( gHUD.m_hsprCursor ), 0 ))
	{
		return;
	}
	
	// Create a triangle, sigh
	gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
	gEngfuncs.pTriAPI->CullFace( TRI_NONE );
	gEngfuncs.pTriAPI->Begin( TRI_QUADS );
	// Overload p->color with index into tracer palette, p->packedColor with brightness
	gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, 1.0 );
	// UNDONE: This gouraud shading causes tracers to disappear on some cards (permedia2)
	gEngfuncs.pTriAPI->Brightness( 1 );
	gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
	gEngfuncs.pTriAPI->Vertex3f( org.x, org.y, org.z );

	gEngfuncs.pTriAPI->Brightness( 1 );
	gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
	gEngfuncs.pTriAPI->Vertex3f( org.x, org.y + 50, org.z );

	gEngfuncs.pTriAPI->Brightness( 1 );
	gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
	gEngfuncs.pTriAPI->Vertex3f( org.x + 50, org.y + 50, org.z );

	gEngfuncs.pTriAPI->Brightness( 1 );
	gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
	gEngfuncs.pTriAPI->Vertex3f( org.x + 50, org.y, org.z );

	gEngfuncs.pTriAPI->End();
	gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
}

#endif

/*
=================
HUD_DrawNormalTriangles

Non-transparent triangles-- add them here
=================
*/
void DLLEXPORT HUD_DrawNormalTriangles( void )
{
	gHUD.m_Spectator.DrawOverview();
	gpRenderFuncs.DrawTrace();
	gpRenderFuncs.DrawDamageTrace();

	if( !(CVAR_GET_FLOAT("r_nowalls") || CVAR_GET_FLOAT("r_textureid")) ) return;
	if( !CVAR_GET_FLOAT("r_nowalls") ) {
		iTexId = CVAR_GET_FLOAT("r_textureid");
		bFound = TRUE;
	}

	if( !bFound ) {
		for( INT i = 0; i < IEngineStudio.GetModelByIndex( 1 )->numtexinfo; i++ ) {
			if( !stricmp( "sky", IEngineStudio.GetModelByIndex( 1 )->texinfo[i].texture->name) ) {
				iTexId = i; bFound = TRUE;
				break;
			}
		}
	}
	for( INT i = 0; i < IEngineStudio.GetModelByIndex( 1 )->numtexinfo; i++ ) {
		if( stricmp(IEngineStudio.GetModelByIndex( 1 )->texinfo[i].texture->name, "SKY") )
			IEngineStudio.GetModelByIndex( 1 )->texinfo[i] = IEngineStudio.GetModelByIndex( 1 )->texinfo[iTexId];
	}
	//gpRenderFuncs.WireframeForModel( IEngineStudio.GetModelByIndex( 1 ) );

#if defined( TEST_IT )
//	Draw_Triangles();
#endif
}

/*
=================
HUD_DrawTransparentTriangles

Render any triangles with transparent rendermode needs here
=================
*/
void DLLEXPORT HUD_DrawTransparentTriangles( void )
{
#if defined( TEST_IT )
//	Draw_Triangles();
#endif
}