#include "sharpfuncs.h"
#ifndef M_PI
	#define M_PI 3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#pragma comment( lib, "urlmon.lib" )

extern Vector pVecDrawWeaponsTrace[128];
extern Vector pVecShootLoc[128];
extern Vector pGaussDmgOrigin[10];
extern FLOAT m_gFlGaussDmg[10];
extern INT pIndicatorColors[10];
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

void CRenderFuncs::DrawDamageTrace( void ) {
	if( !m_gFlGaussDmg[0] ) return;
	glBindTexture( GL_TEXTURE_2D, NULL );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glColor4ub( 255, 0, 0, 100 );

	for( INT d = 0; m_gFlGaussDmg[d] && d < 9; d++ ) {
		/*if( pIndicatorColors[d] ) {
			glColor4ub( 0, 0, 255, 100 );
			glDisable( GL_DEPTH_TEST );
		}*/
		if( pIndicatorColors[d] ) glColor4ub( 0, 0, 255, 100 );
		double r = m_gFlGaussDmg[d]/2;
		for( INT i = 0; i <= 10; i++ ) {
			double lat0 = M_PI*(-0.5+(double)(i-1)/10);
			double z0 = sin( lat0 );
			double zr0 = cos( lat0 );

			double lat1 = M_PI*(-0.5+(double)i/10);
			double z1 = sin( lat1 );
			double zr1 = cos( lat1 );
			glBegin( GL_TRIANGLE_STRIP );
			for( INT j = 0; j <= 10; j++ ) {
				double lng = 2*M_PI*(double)(j-1)/10;
				double x = cos( lng );
				double y = sin( lng );

				glNormal3f((x * zr0)+pGaussDmgOrigin[d].x, (y * zr0)+ pGaussDmgOrigin[d].y, z0+pGaussDmgOrigin[d].z);
				glVertex3f((r * x * zr0) + pGaussDmgOrigin[d].x, (r* y * zr0) + pGaussDmgOrigin[d].y, (r*z0) + pGaussDmgOrigin[d].z);
				glNormal3f((x * zr1) + pGaussDmgOrigin[d].x, (y * zr1) + pGaussDmgOrigin[d].y, z1 + pGaussDmgOrigin[d].z);
				glVertex3f((r* x * zr1) + pGaussDmgOrigin[d].x, (r * y * zr1) + pGaussDmgOrigin[d].y, (r*z1) + pGaussDmgOrigin[d].z);
			}
			glEnd();
		}
		//if( pIndicatorColors[d] ) glEnable( GL_DEPTH_TEST );
	}
}

void CRenderFuncs::DrawCross( Vector origin ) {
	glBindTexture( GL_TEXTURE_2D, NULL );
	glColor3ub( 255, 0, 0 );
	glLineWidth( 3 );

	glBegin( GL_LINES );
	glVertex3f( origin.x, origin.y, origin.z );
	glVertex3f( origin.x, origin.y, origin.z+32 );
	glVertex3f( origin.x, origin.y, origin.z );
	glVertex3f( origin.x, origin.y+16, origin.z );
	glVertex3f( origin.x, origin.y, origin.z );
	glVertex3f( origin.x, origin.y-16, origin.z );
	glVertex3f( origin.x, origin.y, origin.z );
	glVertex3f( origin.x, origin.y, origin.z-32 );
	glEnd();
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