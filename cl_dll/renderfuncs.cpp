#include "renderfuncs.h"

//extern engine_studio_api_t IEngineStudio;

void CRenderFuncs::WireframeForModel( model_s* mdl ) {
#ifndef _DEBUG
	return;
#endif
	sscanf( CVAR_GET_STRING("r_glwireframecolors"), "%i %i %i %i", &pWireColor[0], &pWireColor[1], &pWireColor[2], &pWireColor[3] );
    if( mdl == NULL )
        return;
	glEnableClientState(GL_VERTEX_ARRAY);
	glColor4ub( pWireColor[0], pWireColor[1], pWireColor[2], pWireColor[3] );
	glLineWidth( CVAR_GET_FLOAT("r_glwireframewidth") );
	for( int i = 0; i < mdl->numedges; i++ ) {
        medge_t currentEdge = mdl->edges[i];
        mvertex_t currentVertexes[2] =
        {
            mdl->vertexes[currentEdge.v[0]],
            mdl->vertexes[currentEdge.v[1]]
        };
		pVertexOrigins.push_back( currentVertexes[0].position.x );
		pVertexOrigins.push_back( currentVertexes[0].position.y );
		pVertexOrigins.push_back( currentVertexes[0].position.z );
		pVertexOrigins.push_back( currentVertexes[1].position.x );
		pVertexOrigins.push_back( currentVertexes[1].position.y );
		pVertexOrigins.push_back( currentVertexes[1].position.z );
	}
	glBindTexture(GL_TEXTURE_2D,0);
	glVertexPointer( 3, GL_FLOAT, 0, &pVertexOrigins[0] );
	glDrawArrays( GL_LINES, 0, mdl->numedges*2 );
	pVertexOrigins.clear();
}

CRenderFuncs gpRenderFuncs;