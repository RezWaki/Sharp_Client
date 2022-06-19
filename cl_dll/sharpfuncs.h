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
#include "pmtrace.h"
#include <Windows.h>
#include <vector>
#include <fstream>
#include <sys\types.h>
#include <sys\stat.h>
#ifdef _DEBUG
#include <GL\glew.h>
#endif
#include <GL\GL.h>
#include <GL\GLU.h>
#define CL_VER "13.0"
#define FNT_SIZE 16

#ifdef _DEBUG
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glew32.lib" )
#endif //glew?? dont need if its release

class CRenderFuncs{
	private:
		INT pWireColor[4];
		std::vector<float> pVertexOrigins;
		std::string pModelName;
		char pModelPath[256];
		char pNewModelPath[256];
		INT pCounter;
	public:
		char* GetRealModelPath( char* input );
		void WireframeForModel( model_s* mdl );
		INT* ReadBitmapArray( const char* file, INT width, INT height );
		void DrawBmpImage( const char* file, INT x, INT y );
		void LensEffect( model_s* mdl );
		void DrawTrace( void );
		void DrawTriTrace( void );
		void DrawDamageTrace( void );
		void DrawCross( Vector origin );
};

#pragma pack(push, 1)
struct bmp_header {
	char magic[2];
	INT fileSize;
	INT res;
	INT offset;
	INT header_size;
	INT width;
	INT height;
	INT bits_per_pixel;
};
#pragma pack(pop)

class CFonts{
	private:
		INT iFontSize;
		INT iMaxLetters;
		BOOL bFontsFound;
		char* GetFontPath( char* letter );
		std::vector<unsigned char> ReadFont( char* letter );
	public:
		void CopyCharArray( char** dest, char** src );
		void SetFontSize( INT size );
		void DrawHudString( INT x, INT y, char* text, INT r, INT g, INT b, INT a );
		void Init( void );
		CFonts( void ) { iFontSize = 16; iMaxLetters = 0; bFontsFound = FALSE; };
		~CFonts( void ) { };
};