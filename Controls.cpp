#include "Controls.h"

Controls::Controls(HINSTANCE hinstance, HWND hWnd)
{
	this->hinstance = hinstance;
	this->hWnd = hWnd;
}

Controls::~Controls()
{
	if (g_keyboard_device)
	{
		g_keyboard_device->Unacquire();
		g_keyboard_device->Release();
	}

	if (g_direct_input) g_direct_input->Release();
}

HRESULT Controls::InitialiseInput()
{
	HRESULT hr;
	ZeroMemory(g_keyboard_keys_state, sizeof(g_keyboard_keys_state));

	hr = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_direct_input, NULL);
	hr = g_direct_input->CreateDevice(GUID_SysKeyboard, &g_keyboard_device, NULL);

	if (FAILED(hr))
		return hr;

	hr = g_keyboard_device->SetDataFormat(&c_dfDIKeyboard);

	if (FAILED(hr))
		return hr;

	hr = g_keyboard_device->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	if (FAILED(hr))
		return hr;

	hr = g_keyboard_device->Acquire();

	if (FAILED(hr))
		return hr;

	return S_OK;
}

void Controls::ReadInputStates()
{
	HRESULT hr;

	hr = g_keyboard_device->GetDeviceState(sizeof(g_keyboard_keys_state), (LPVOID)&g_keyboard_keys_state);

	if (FAILED(hr))
	{
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
			g_keyboard_device->Acquire();
	}
}

boolean Controls::IsKeyPressed(unsigned char DI_keycode)
{
	return g_keyboard_keys_state[DI_keycode] & 0x80;
}
