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
#include <Windows.h>
#include <vector>
#ifdef _DEBUG
#include <GL\glew.h>
#endif

#define CL_VER "10.0"

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
};