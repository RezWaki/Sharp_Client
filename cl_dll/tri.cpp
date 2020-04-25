//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// Triangle rendering, if any

#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "com_model.h"
#include "studio.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "dlight.h"
#include "triangleapi.h"
#include "r_studioint.h"
#include "StudioModelRenderer.h"
#include "GameStudioModelRenderer.h"

// Triangle rendering apis are in gEngfuncs.pTriAPI



#define DLLEXPORT __declspec( dllexport )

extern "C"
{
	void DLLEXPORT HUD_DrawNormalTriangles( void );
	void DLLEXPORT HUD_DrawTransparentTriangles( void );
};

extern engine_studio_api_t IEngineStudio;

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

//color24* newColors;

void RemoveLights( void ) {
	//newColors->r = newColors->g = newColors->b = 255;
	/*model_s* pMap = IEngineStudio.GetModelByIndex(1);
    if ((pMap == NULL) || (pMap->type != mod_brush))
        return;
	pMap->lightdata->r = pMap->lightdata->g = pMap->lightdata->b = 255;
	strcpy( pMap->name, "models/player/hgrunt/hgrunt.mdl" );*/
    // all valid model_s' stored in the engine start at 1 by this method, not 0,
    // and the world model is always the first model.  Thus, it's at index 1.
	gEngfuncs.pTriAPI->End();
    model_s *pModel = IEngineStudio.GetModelByIndex (1);

    // make sure it's a valid model_s pointer and the model type is mod_brush
    if ((pModel == NULL) || (pModel->type != mod_brush))
        return;

	IEngineStudio.SetRenderModel( pModel );

	pModel->lightdata->r = 255;
	pModel->lightdata->g = pModel->lightdata->b = 0;

    // now it shouldn't crash if we do this
    // we're simply drawing every single edge in the world
    gEngfuncs.pTriAPI->Begin (TRI_LINES);
    gEngfuncs.pTriAPI->CullFace (TRI_NONE); // don't cull
    gEngfuncs.pTriAPI->RenderMode (kRenderNormal);  // normal render mode
    gEngfuncs.pTriAPI->Color4ub (255, 255, 255, 255);   // white lines

    // for all edges in the map
    for (int i = 0; i < pModel->numedges; i++)
    {
        // get the current edge at index i
        medge_t currentEdge = pModel->edges[i];
        // the medge_t's simply store indexes into a master vertex (mvertex_t) list,
        // so get those two vertexes
        mvertex_t currentVertexes[2] =
        {
            pModel->vertexes[currentEdge.v[0]],
            pModel->vertexes[currentEdge.v[1]]
        };

        // now render this edge/line
        gEngfuncs.pTriAPI->Vertex3fv (currentVertexes[0].position);
        gEngfuncs.pTriAPI->Vertex3fv (currentVertexes[1].position);
    }

    gEngfuncs.pTriAPI->End ();  // we're done rendering
}

/*
=================
HUD_DrawNormalTriangles

Non-transparent triangles-- add them here
=================
*/
void DLLEXPORT HUD_DrawNormalTriangles( void )
{
	//RemoveLights();
	gHUD.m_Spectator.DrawOverview();
	//RemoveLights();
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