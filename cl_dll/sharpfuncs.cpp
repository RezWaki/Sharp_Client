#include "sharpfuncs.h"

//extern engine_studio_api_t IEngineStudio;
extern Vector pVecDrawWeaponsTrace[128];
extern Vector pVecShootLoc[128];
//extern cl_enginefunc_t gEngfuncs;
INT pTraceLineColors[3];
Vector pVecXhairTrace, fwd, rgt, up, dest, vwangles;

char* specsmb[22] = { "?", "/", "\\", "(", ")", "!", "~", "`", "@", "#", ":", "+", "-", "*",
	"&", "$", "%", "<", ">", ",", ".", "^" };
char* specnames[22] = { "qstnmrk", "slash", "bkslash", "opnbrk", "clsbrk", "excl", "tilda", "ew",
	"at", "hash", "colon", "plus", "minus", "star",
	"ampersant", "dlr", "percnt", "opntribrk", "clstribrk", "comma", "dot", "uparrow" };

void CRenderFuncs::WireframeForModel( model_s* mdl ) {
#ifndef _DEBUG
	return;
#endif
#ifdef _DEBUG
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
#endif
}

char* CRenderFuncs::GetRealModelPath( char* input ) {
	if( CVAR_GET_STRING("cl_models_subfolder") != "0" ) {
		pModelName = input;
		for( pCounter = 0; pCounter < pModelName.length(); pCounter++ ) {
			if( pModelName.substr( pCounter, 1 ) == "/" ) {
				break;
			}
		}
		strcpy( pModelPath, pModelName.substr( pCounter+1, pModelName.length()-(pCounter+1) ).c_str() );
		sprintf( pNewModelPath, "models/%s/%s", CVAR_GET_STRING("cl_models_subfolder"), pModelPath );
		gEngfuncs.pfnCenterPrint( pNewModelPath );
		gEngfuncs.pfnConsolePrint( pNewModelPath );
		return pNewModelPath;
	}
	else return input;
}

//opengl
void CRenderFuncs::DrawTrace( void ) {
	if( CVAR_GET_FLOAT("cl_maxtracelines") > 128 ) gEngfuncs.Cvar_SetValue( "cl_maxtracelines", 128 );

	if( CVAR_GET_FLOAT("cl_traceline") ) {
		for( INT y = 0; y < CVAR_GET_FLOAT("cl_maxtracelines"); y++ ) {
			if( pVecDrawWeaponsTrace[y].x != 0 && pVecDrawWeaponsTrace[y].y != 0 && pVecDrawWeaponsTrace[y].z != 0 ) {
				glBindTexture( GL_TEXTURE_2D, NULL );
				glBegin( GL_LINES );
				sscanf( CVAR_GET_STRING("cl_traceline_color"), "%i %i %i", &pTraceLineColors[0], &pTraceLineColors[1], &pTraceLineColors[2] );
				glColor3ub( pTraceLineColors[0], pTraceLineColors[1], pTraceLineColors[2] );
				for( INT i = 0; i < CVAR_GET_FLOAT("cl_traceline"); i++ ) {
					glVertex3f( pVecShootLoc[y].x, pVecShootLoc[y].y, pVecShootLoc[y].z );
					glVertex3f( pVecDrawWeaponsTrace[y].x, pVecDrawWeaponsTrace[y].y, pVecDrawWeaponsTrace[y].z );
					glVertex3f( pVecShootLoc[y].x+i, pVecShootLoc[y].y+i, pVecShootLoc[y].z+i );
					glVertex3f( pVecDrawWeaponsTrace[y].x+i, pVecDrawWeaponsTrace[y].y+i, pVecDrawWeaponsTrace[y].z+i );
				} //draw 6 lines
				glEnd();
			}
		}
	}

	gEngfuncs.GetViewAngles( vwangles );
	AngleVectors( vwangles, fwd, rgt, up );
	VectorMA( gEngfuncs.GetLocalPlayer()->origin, 4098, fwd, dest );
	pVecXhairTrace = gEngfuncs.PM_TraceLine( gEngfuncs.GetLocalPlayer()->origin, dest, 1, 2, -1 )->endpos;

	if( CVAR_GET_FLOAT("cl_cross_trace") ) {
		glBindTexture( GL_TEXTURE_2D, NULL );
		glBegin( GL_LINES );
		sscanf( CVAR_GET_STRING("cl_crosscolor"), "%i %i %i", &pTraceLineColors[0], &pTraceLineColors[1], &pTraceLineColors[2] );
		glColor3ub( pTraceLineColors[0], pTraceLineColors[1], pTraceLineColors[2] );
		for( INT i = 0; i < CVAR_GET_FLOAT("cl_cross_trace"); i++ ) {
			glVertex3f( gEngfuncs.GetLocalPlayer()->origin.x, gEngfuncs.GetLocalPlayer()->origin.y, gEngfuncs.GetLocalPlayer()->origin.z );
			glVertex3f( pVecXhairTrace.x, pVecXhairTrace.y, pVecXhairTrace.z );
		} //draw 6 lines
		glEnd();
	}
}

//triangle api (engine)
//might dump that since steam doesnt support d3d anymore
//and hl's triapi doesnt work properly at all
void CRenderFuncs::DrawTriTrace( void ) {
	if( CVAR_GET_FLOAT("cl_maxtracelines") > 128 ) gEngfuncs.Cvar_SetValue( "cl_maxtracelines", 128 );

	if( CVAR_GET_FLOAT("cl_traceline") ) {
		for( INT y = 0; y < CVAR_GET_FLOAT("cl_maxtracelines"); y++ ) {
			if( pVecDrawWeaponsTrace[y].x != 0 && pVecDrawWeaponsTrace[y].y != 0 && pVecDrawWeaponsTrace[y].z != 0 ) {
				gEngfuncs.pTriAPI->Begin( TRI_LINES );
				sscanf( CVAR_GET_STRING("cl_traceline_color"), "%i %i %i", &pTraceLineColors[0], &pTraceLineColors[1], &pTraceLineColors[2] );
				gEngfuncs.pTriAPI->Color4ub( pTraceLineColors[0], pTraceLineColors[1], pTraceLineColors[2], 255 );
				for( INT i = 0; i < CVAR_GET_FLOAT("cl_traceline"); i++ ) {
					gEngfuncs.pTriAPI->Vertex3f( pVecShootLoc[y].x, pVecShootLoc[y].y, pVecShootLoc[y].z );
					gEngfuncs.pTriAPI->Vertex3f( pVecDrawWeaponsTrace[y].x, pVecDrawWeaponsTrace[y].y, pVecDrawWeaponsTrace[y].z );
					gEngfuncs.pTriAPI->Vertex3f( pVecShootLoc[y].x+i, pVecShootLoc[y].y+i, pVecShootLoc[y].z+i );
					gEngfuncs.pTriAPI->Vertex3f( pVecDrawWeaponsTrace[y].x+i, pVecDrawWeaponsTrace[y].y+i, pVecDrawWeaponsTrace[y].z+i );
				} //draw 6 lines
				gEngfuncs.pTriAPI->End();
			}
		}
	}

	gEngfuncs.GetViewAngles( vwangles );
	AngleVectors( vwangles, fwd, rgt, up );
	VectorMA( gEngfuncs.GetLocalPlayer()->origin, 4098, fwd, dest );
	pVecXhairTrace = gEngfuncs.PM_TraceLine( gEngfuncs.GetLocalPlayer()->origin, dest, 1, 2, -1 )->endpos;

	if( CVAR_GET_FLOAT("cl_cross_trace") ) {
		gEngfuncs.pTriAPI->Begin( TRI_LINES );
		sscanf( CVAR_GET_STRING("cl_crosscolor"), "%i %i %i", &pTraceLineColors[0], &pTraceLineColors[1], &pTraceLineColors[2] );
		gEngfuncs.pTriAPI->Color4ub( pTraceLineColors[0], pTraceLineColors[1], pTraceLineColors[2], 255 );
		for( INT i = 0; i < CVAR_GET_FLOAT("cl_cross_trace"); i++ ) {
			gEngfuncs.pTriAPI->Vertex3f( gEngfuncs.GetLocalPlayer()->origin.x, gEngfuncs.GetLocalPlayer()->origin.y, gEngfuncs.GetLocalPlayer()->origin.z );
			gEngfuncs.pTriAPI->Vertex3f( pVecXhairTrace.x, pVecXhairTrace.y, pVecXhairTrace.z );
		} //draw the specified number of lines
		gEngfuncs.pTriAPI->End();
	}
}

void CFonts::Init( void ) {
	struct stat pDirStat;
	INT i = 0;
	iFontSize = 16;
	iMaxLetters = i;
	if( stat("valve\\fonts", &pDirStat) ) {
		gEngfuncs.pfnClientCmd( "echo \"Can't use valve\\fonts, probably the folder doesn't exist!\"" );
		gEngfuncs.pfnClientCmd( "echo \"gpFonts.Init() failed\"" );
		return;
	}
	bFontsFound = TRUE;
}

void CFonts::SetFontSize( INT size ) {
	iFontSize = size;
}

char* CFonts::GetFontPath( char* letter ) {
	BOOL bOneMatch = FALSE;
	char pBitmapPath[256];
	for( INT i = 0; i < 21; i++ ) {
		if( !stricmp(letter, specsmb[i]) ) {
			sprintf( pBitmapPath, "valve\\fonts\\%s.bmp", specnames[i] );
			bOneMatch = TRUE;
		}
	}
	if( !bOneMatch )
		sprintf( pBitmapPath, "valve\\fonts\\%s.bmp", letter );

	return pBitmapPath;
}

std::vector<unsigned char> CFonts::ReadFont( char* letter ) {
	if( bFontsFound ) {
		char fname[256];
		INT i, j, k, rev_j;
		bmp_header head;
		strcpy( fname, GetFontPath(letter) );
		std::ifstream f( fname, std::ios::binary );

		f.read( (char*)&head, sizeof(head) );

		int lineSize = (iFontSize/8+(iFontSize/8)%4);
		int fileSize = lineSize * iFontSize;

		std::vector<unsigned char> img(iFontSize*iFontSize);
		std::vector<unsigned char> data(fileSize);

		f.seekg( head.offset );

		f.read( (char*)&data[0], fileSize );

		for( j = 0, rev_j = iFontSize-1; j < iFontSize; j++, rev_j-- ) {
			for( i = 0; i < iFontSize/8; i++ ) {
				int fpos = j*lineSize+i, pos = rev_j*iFontSize+i*8;
				for( k = 0; k < 8; k++ )
					img[pos+(7-k)] = (data[fpos] >> k) & 1;
			}
		}
		return img;
	}
	else{
		return std::vector<unsigned char>(0, 0);
	}
}

void CFonts::DrawHudString( INT x, INT y, char* text, INT r, INT g, INT b, INT a ) {
	if( bFontsFound ) {
		std::vector<unsigned char> pImage;
		std::string* pText = new std::string( text );
		char* pLetter = new char;
		INT x_offset = 0;
		for( INT i = 0; i < pText->length(); i++ ) {
			strcpy( pLetter, (char*)pText->substr(i, 1).c_str() );
			if( !strcmp( pLetter, " " ) ) { //spacebar
				x_offset += 16;
				continue;
			}
			pImage = ReadFont( pLetter );
			for( INT w = 0; w < iFontSize; w++ ) {
				for( INT z = 0; z < iFontSize; z++ ) {
					if( pImage[z+w*iFontSize] ) { //z+w*font_size = z, w
						FillRGBA( (x+z)+x_offset, y+w, 1, 1, r, g, b, a );
					}
				}
			}
			x_offset += iFontSize;
		}
		delete[] pLetter;
		delete pText;
	}
}

CFonts		 gpFonts;
CRenderFuncs gpRenderFuncs;