#include <Windows.h>
#include <strsafe.h>

class CHLRawInput {
	public:
		HRAWINPUT hRawInput;
		RAWINPUT* raw;
		RAWINPUTDEVICE pMouse;
		RAWINPUT* pRawInput;
		UINT pSize;
		BOOL bInitResult;
		WNDPROC hWndProc;
		HWND hWindow;
		HWND hTempWnd;
		INT pos[2];
		BOOL RI_Init( char* window_title );
		void RI_HookWindow( void );
};