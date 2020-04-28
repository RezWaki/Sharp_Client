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

/*
#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "com_model.h"
#include "studio.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "dlight.h"
#include "triangleapi.h"
#include "pmtrace.h"

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <Windows.h>

#include "studio_util.h"
#include "r_studioint.h"

#include "StudioModelRenderer.h"
#include "GameStudioModelRenderer.h"
*/

#define CL_VER "7.0"

#ifdef _DEBUG
#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glew32.lib" )
#endif //glew?? dont need if its release

class CRenderFuncs{
	private:
		INT pWireColor[4];
		std::vector<float> pVertexOrigins;
	public:
		void WireframeForModel( model_s* mdl );
};