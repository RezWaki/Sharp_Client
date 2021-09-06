#include "hl_rawinput.h"

CHLRawInput gHLRawInput;

BOOL CHLRawInput::RI_Init( char* window_title ) {
	bInitResult = FALSE;
	hWindow = FindWindowA( NULL, window_title );
	hTempWnd = hWindow;

	pMouse.usUsagePage = 0x01;
	pMouse.usUsage = 0x02;
	pMouse.dwFlags = RIDEV_INPUTSINK;
	pMouse.hwndTarget = hTempWnd;
	if( RegisterRawInputDevices(&pMouse, 1, sizeof(pMouse)) ) bInitResult = TRUE;
	
	RI_HookWindow();
	return bInitResult;
}

LRESULT CALLBACK RI_WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	if( uMsg == WM_INPUT ) {
		gHLRawInput.hRawInput = (HRAWINPUT)lParam;
		GetRawInputData( gHLRawInput.hRawInput, RID_INPUT, NULL, &gHLRawInput.pSize, sizeof(RAWINPUTHEADER) );
		BYTE* pByte = new BYTE[gHLRawInput.pSize];
		GetRawInputData( (HRAWINPUT)lParam, RID_INPUT, pByte, &gHLRawInput.pSize, sizeof(RAWINPUTHEADER) );
		gHLRawInput.raw = (RAWINPUT*)pByte;
		if( gHLRawInput.raw->header.dwType == RIM_TYPEMOUSE ) {
			gHLRawInput.pos[0] = gHLRawInput.raw->data.mouse.lLastX;
			gHLRawInput.pos[1] = gHLRawInput.raw->data.mouse.lLastY;
		}
		delete[] pByte;
	}
	return CallWindowProcA( gHLRawInput.hWndProc, hWnd, uMsg, wParam, lParam );
}

void CHLRawInput::RI_HookWindow( void ) {
	hWndProc = (WNDPROC)SetWindowLongA( hTempWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)RI_WindowProc );
}