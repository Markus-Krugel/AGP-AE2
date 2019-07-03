#pragma once
#include <dinput.h>
#include "camera.h"

class Controls
{
	public:
		Controls();
		Controls(HINSTANCE hinstance, HWND hWnd);
		~Controls();
		HRESULT InitialiseInput();
		void ReadInputStates();
		boolean IsKeyPressed(unsigned char DI_keycode);

	protected:
		IDirectInput8* g_direct_input;
		IDirectInputDevice8* g_keyboard_device;
		unsigned char g_keyboard_keys_state[256];
		HINSTANCE	hinstance;
		HWND		hWnd;
};