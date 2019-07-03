//The #include order is important
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <windows.h>
#include <dxerr.h>
#include <time.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>
#include "camera.h"
#include "text2D.h"
#include "Model.h"

//////////////////////////////////////////////////////////////////////////////////////
//	Global Variables
//////////////////////////////////////////////////////////////////////////////////////
HINSTANCE	g_hInst = NULL;
HWND		g_hWnd = NULL;

D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pD3DDevice = NULL;
ID3D11DeviceContext*    g_pImmediateContext = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;

ID3D11RenderTargetView* g_pBackBufferRTView = NULL;

bool colorChosen = false;
float rgba_clear_colour[4] = {};

// Rename for each tutorial – This will appear in the title bar of the window
char		g_TutorialName[100] = "Tutorial 01 Exercise 01\0";

ID3D11Buffer*		g_pVertexBuffer;
ID3D11Buffer*		g_pVertexBuffer2;
ID3D11VertexShader*	g_pVertexShader;
ID3D11PixelShader*	g_pPixelShader;
ID3D11InputLayout*	g_pInputLayout;

ID3D11Buffer* g_pConstantBuffer0;
ID3D11Buffer* g_pConstantBuffer1;
float redAmount = 0.1f;

XMMATRIX projection, world, world2, view;

ID3D11DepthStencilView* g_pZBuffer;

Camera* camera;

ID3D11ShaderResourceView* g_pTexture0;
ID3D11SamplerState* g_pSampler0;

Text2D* g_p2DText;

XMVECTOR g_directional_light_shines_from;
XMVECTOR g_directional_light_colour;
XMVECTOR g_ambient_light_colour;
XMVECTOR g_directional_light_rotation;

XMVECTOR g_point_light_colour;
XMVECTOR g_point_light_position;

Model* sphere;
Model* sphere2;

//XMMATRIX rotation, translation, scale, world_transform;
//
//scale = XMMatrixScaling(1, 1, 2);
//// rotate 45 degrees around X axis, need to convert to Radians
//rotation = XMMatrixRotationX(XMConvertToRadians(415.0));
//translation = XMMatrixTranslation(0, 0, 5);
//
//world transform = scale * rotation * translation;
//
//XMMATRIX view;
//view = XMMatrixIdentity();
//
//XMMATRIX projection;
//projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), 640.0 / 480.0, 1.0, 100.0);
//
//XMMATRIX WorldViewProjection = world * view * projection;

//Define vertex structure
struct POS_COL_TEX__NORM_VERTEX//This will be added to and renamed in future tutorials
{
	XMFLOAT3	pos;
	XMFLOAT4	Col;
	XMFLOAT2 Texture0;
	XMFLOAT3 Normal;
};

POS_COL_TEX__NORM_VERTEX vertices[36] =
{
	// back face 
	{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f), 	XMFLOAT3(0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(0.0f, 0.0f, 1.0f)},
																							
	// front face																			
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f)},
	{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f)},
																							
	// left face																			
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f)},
																							
	// right face																			
	{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f)},
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f)},
																							
	// bottom face																			
	{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)},
																	
	// top face														
	{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f)},
	{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f)}
};

POS_COL_TEX__NORM_VERTEX box2[36] =
{
	// back face 
	{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.8f, 1.0f),		XMFLOAT2(0.0f, 0.0f), 	XMFLOAT3(0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.8f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.8f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.8f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.8f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 0.0f, 1.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.8f, 1.0f),		XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(0.0f, 0.0f, 1.0f)},

	// front face																			
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.8f, 0.0f, 0.8f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.8f, 0.0f, 0.8f, 1.0f),	XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.8f, 0.0f, 0.8f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f)},
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.8f, 0.0f, 0.8f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.8f, 0.0f, 0.8f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f)},
	{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.8f, 0.0f, 0.8f, 1.0f),	XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(0.0f, 0.0f, -1.0f)},

	// left face																			
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.8f, 0.8f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.8f, 0.8f, 1.0f),	XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.8f, 0.8f, 1.0f),	XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.8f, 0.8f, 1.0f),	XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.8f, 0.8f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.8f, 0.8f, 1.0f),	XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(-1.0f, 0.0f, 0.0f)},

	// right face																			
	{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f)},
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.8f, 0.0f, 1.0f),	XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(1.0f, 0.0f, 0.0f)},

	// bottom face																			
	{XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f),	XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)},
	{XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f),	XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, -1.0f, 0.0f)},

	// top face														
	{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.8f, 0.8f, 0.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f)},
	{XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.8f, 0.8f, 0.0f, 1.0f),		XMFLOAT2(1.0f, 1.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.8f, 0.8f, 0.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(0.8f, 0.8f, 0.0f, 1.0f),		XMFLOAT2(0.0f, 0.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f)},
	{XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.8f, 0.8f, 0.0f, 1.0f),		XMFLOAT2(1.0f, 0.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f)},
	{XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.8f, 0.8f, 0.0f, 1.0f),	XMFLOAT2(0.0f, 1.0f),	XMFLOAT3(0.0f, 1.0f, 0.0f)}
};

struct CONSTANT_BUFFER0
{
	XMMATRIX WorldViewProjection;
	XMVECTOR directional_light_vector;
	XMVECTOR directional_light_colour;
	XMVECTOR ambient_light_colour;
	float RedAmount;
	float scaleAmount;
	XMVECTOR g_point_light_colour;
	XMVECTOR g_point_light_position;
	XMFLOAT2 packing_bytes;
};


//////////////////////////////////////////////////////////////////////////////////////
//	Forward declarations
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HRESULT InitialiseD3D();
void ShutdownD3D();

void RenderFrame(void);

void Update();

void CreateMatrices();

HRESULT InitialiseGraphics(void);

//////////////////////////////////////////////////////////////////////////////////////
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(time(NULL));

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitialiseWindow(hInstance, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	if (FAILED(InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		return 0;
	}

	if (FAILED(InitialiseGraphics()))
	{
		DXTRACE_MSG("Failed to initialise graphics");
		return 0;
	}

	// Main message loop
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			RenderFrame();			
		}
	}

	ShutdownD3D();

	return (int)msg.wParam;
}


//////////////////////////////////////////////////////////////////////////////////////
// Register class and create window
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Give your app your own name
	char Name[100] = "Hello World\0";

	// Register class
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//   wcex.hbrBackground = (HBRUSH )( COLOR_WINDOW + 1); // Needed for non-D3D apps
	wcex.lpszClassName = Name;

	if (!RegisterClassEx(&wcex)) return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(Name, g_TutorialName, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
		rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
// Called every time the application receives a message
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(g_hWnd);
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
// Create D3D device and swap chain
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseD3D()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, // comment out this line if you need to test D3D 11.0 functionality on hardware that doesn't support it
		D3D_DRIVER_TYPE_WARP, // comment this out also to use reference device
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &g_pSwapChain,
			&g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Get pointer to back buffer texture
	ID3D11Texture2D *pBackBufferTexture;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) return hr;

	// Use the back buffer texture pointer to create the render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,
		&g_pBackBufferRTView);
	pBackBufferTexture->Release();

	if (FAILED(hr)) return hr;

	// Create a Z Buffer texture
	D3D11_TEXTURE2D_DESC text2dDesc;
	ZeroMemory(&text2dDesc, sizeof(text2dDesc));

	text2dDesc.Width = width;
	text2dDesc.Height = height;
	text2dDesc.ArraySize = 1;
	text2dDesc.MipLevels = 1;
	text2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	text2dDesc.SampleDesc.Count = sd.SampleDesc.Count;
	text2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	text2dDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D *pZBufferTexture;
	hr = g_pD3DDevice->CreateTexture2D(&text2dDesc, NULL, &pZBufferTexture);

	if (FAILED(hr)) return hr;

	// Create the Z buffer

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = text2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	g_pD3DDevice->CreateDepthStencilView(pZBufferTexture, &dsvDesc, &g_pZBuffer);
	pZBufferTexture->Release();

	// Set the render target view
	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, g_pZBuffer);

	// Set the viewport
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_pImmediateContext->RSSetViewports(1, &viewport);

	g_p2DText = new Text2D("Assets/font1.bmp", g_pD3DDevice, g_pImmediateContext);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
// Clean up D3D objects
//////////////////////////////////////////////////////////////////////////////////////
void ShutdownD3D()
{
	if (g_pVertexBuffer) g_pVertexBuffer->Release();
	if (g_pVertexBuffer2) g_pVertexBuffer2->Release();
	if (g_pInputLayout) g_pInputLayout->Release();
	if (g_pVertexShader) g_pVertexShader->Release();
	if (g_pPixelShader) g_pPixelShader->Release();

	if (g_pBackBufferRTView) g_pBackBufferRTView->Release();

	if (g_pConstantBuffer0) g_pConstantBuffer0->Release();
	if (g_pConstantBuffer1) g_pConstantBuffer1->Release();

	if (g_pTexture0) g_pTexture0->Release();
	if (g_pSampler0) g_pSampler0->Release();

	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();

	if (g_pD3DDevice) g_pD3DDevice->Release();
	if (camera) delete camera;
	if (g_p2DText) delete g_p2DText;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//Init graphics - Tutorial 03
/////////////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseGraphics()
{
	HRESULT hr = S_OK;

	
	//Define vertices of a triangle - screen coordinates -1.0 to +1.0

	//Set up and create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;										//Allows use by CPU and GPU
	bufferDesc.ByteWidth = sizeof(vertices);							//Set the total size of the buffer (in this case, 3 vertices)
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;							//Set the type of buffer to vertex buffer
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							//Allow access by the CPU
	hr = g_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &g_pVertexBuffer);		//Create the buffer

	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 160;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = g_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &g_pConstantBuffer0);

	if (FAILED(hr))//Return an error code if failed
	{
		return hr;
	}

	//Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	//Lock the buffer to allow writing
	g_pImmediateContext->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	//Copy the data
	memcpy(ms.pData, vertices, sizeof(vertices));

	//Unlock the buffer
	g_pImmediateContext->Unmap(g_pVertexBuffer, NULL);




	//Define vertices of a triangle - screen coordinates -1.0 to +1.0

	//Set up and create vertex buffer
	D3D11_BUFFER_DESC bufferDesc2;
	ZeroMemory(&bufferDesc2, sizeof(bufferDesc2));
	bufferDesc2.Usage = D3D11_USAGE_DYNAMIC;										//Allows use by CPU and GPU
	bufferDesc2.ByteWidth = sizeof(box2);							//Set the total size of the buffer (in this case, 3 vertices)
	bufferDesc2.BindFlags = D3D11_BIND_VERTEX_BUFFER;							//Set the type of buffer to vertex buffer
	bufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							//Allow access by the CPU
	hr = g_pD3DDevice->CreateBuffer(&bufferDesc2, NULL, &g_pVertexBuffer2);		//Create the buffer

	D3D11_BUFFER_DESC constant_buffer_desc2;
	ZeroMemory(&constant_buffer_desc2, sizeof(constant_buffer_desc2));

	constant_buffer_desc2.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc2.ByteWidth = 80;
	constant_buffer_desc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = g_pD3DDevice->CreateBuffer(&constant_buffer_desc2, NULL, &g_pConstantBuffer1);

	if (FAILED(hr))//Return an error code if failed
	{
		return hr;
	}

	//Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms2;

	//Lock the buffer to allow writing
	g_pImmediateContext->Map(g_pVertexBuffer2, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms2);

	//Copy the data
	memcpy(ms2.pData, box2, sizeof(box2));

	//Unlock the buffer
	g_pImmediateContext->Unmap(g_pVertexBuffer2, NULL);

	//Load and compile the pixel and vertex shaders - use vs_5_0 to target DX11 hardware only
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	//Create shader objects
	hr = g_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVertexShader);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = g_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPixelShader);
	if (FAILED(hr))
	{
		return hr;
	}

	//Set the shader objects as active
	g_pImmediateContext->VSSetShader(g_pVertexShader, 0, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, 0, 0);

	//Create and set the input layout object
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		//Be very careful setting the correct dxgi format and D3D version
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		//NOTE the spelling of COLOR. Again, be careful setting the correct dxgi format (using A32) and correct D3D version
		{"COLOR", 0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = g_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &g_pInputLayout);
	if (FAILED(hr))
	{
		return hr;
	}

	g_pImmediateContext->IASetInputLayout(g_pInputLayout);

	camera = new Camera(1, 1, -0.5f, 0);

	CreateMatrices();

	D3DX11CreateShaderResourceViewFromFile(g_pD3DDevice, "Assets/texture.jpg", NULL, NULL, &g_pTexture0, NULL);

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	g_directional_light_shines_from = XMVectorSet(0, 0, -1, 0);
	g_directional_light_colour = XMVectorSet(0.5f, 1, 0, 0);
	g_ambient_light_colour = XMVectorSet(0.1f, 0.1f, 0.1f, 1);

	g_point_light_colour = XMVectorSet(1, 0, 1, 0);
	g_point_light_position = XMVectorSet(1, 0, 1, 0);

	sphere = new Model(g_pD3DDevice, g_pImmediateContext);
	sphere->LoadObjModel((char*)"Assets/Sphere.obj");
	sphere->AddTexture((char*)"Assets/texture.jpg");

	sphere2 = new Model(g_pD3DDevice, g_pImmediateContext);
	sphere2->LoadObjModel((char*)"Assets/Sphere.obj");
	sphere2->AddTexture((char*)"Assets/texture.jpg");
	sphere2->setPosition(10, 0, 10);

	return S_OK;
	
}
/////////////////////////////////////////////////////////////////////////////////////////////


// Render frame
void RenderFrame(void)
{
	/*CreateMatrices();
	if (!colorChosen)
	{
		rgba_clear_colour[0] = (float)rand() / ((float)RAND_MAX + 1);
		rgba_clear_colour[1] = (float)rand() / ((float)RAND_MAX + 1);
		rgba_clear_colour[2] = (float)rand() / ((float)RAND_MAX + 1);
		rgba_clear_colour[3] = 1.0f;

		colorChosen = true;
	}*/

	rgba_clear_colour[0] = 0.07;
	rgba_clear_colour[1] = 0.71;
	rgba_clear_colour[2] = 0.67;

	//Matrices

	XMMATRIX transpose1;
	XMMATRIX transpose2;

	CONSTANT_BUFFER0 cb0_values;
	CONSTANT_BUFFER0 cb1_values;

	/*transpose1 = XMMatrixTranspose(world);
	transpose2 = XMMatrixTranspose(world2);

	cb0_values.directional_light_colour = g_directional_light_colour;
	cb0_values.ambient_light_colour = g_ambient_light_colour;
	cb0_values.directional_light_vector = XMVector3Transform(g_directional_light_shines_from + g_directional_light_rotation, transpose1);
	cb0_values.directional_light_vector = XMVector3Normalize(cb0_values.directional_light_vector);

	cb1_values.directional_light_colour = g_directional_light_colour;
	cb1_values.ambient_light_colour = g_ambient_light_colour;
	cb1_values.directional_light_vector = XMVector3Transform(g_directional_light_shines_from + g_directional_light_rotation, transpose2);
	cb1_values.directional_light_vector = XMVector3Normalize(cb1_values.directional_light_vector);*/

	//XMVECTOR determinant;
	XMMATRIX inverse;
	//inverse = XMMatrixInverse(&determinant, world);

	/*g_point_light_position = XMVector3Transform(g_point_light_position, inverse);

	cb0_values.g_point_light_colour = g_point_light_colour;
	cb0_values.g_point_light_position = g_point_light_position;

	cb1_values.g_point_light_colour = g_point_light_colour;
	cb1_values.g_point_light_position = g_point_light_position;
*/
	view = camera->GetViewMatrix();
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(95.0), 640.0 / 480.0, 1.0, 100.0);
	/*cb0_values.WorldViewProjection = world * view * projection;
	cb1_values.WorldViewProjection = world2 * view * projection;

	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer0, 0, 0, &cb0_values, 0, 0);

	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer0);*/

	// Clear the back buffer - choose a colour you like
	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView,  rgba_clear_colour);

	g_pImmediateContext->ClearDepthStencilView(g_pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//g_pImmediateContext->PSSetSamplers(0, 1, &g_pSampler0);
	//g_pImmediateContext->PSSetShaderResources(0, 1, &g_pTexture0);

	//// Set vertex buffer //03-01
	//UINT stride = sizeof(POS_COL_TEX__NORM_VERTEX);
	//UINT offset = 0;
	//g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	//g_pImmediateContext->VSSetShader(g_pVertexShader, NULL, 0);
	//g_pImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	//g_pImmediateContext->IASetInputLayout(g_pInputLayout);

	//// Select which primitive type to use //03-01
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//// Draw the vertex buffer to the back buffer //03-01
	//g_pImmediateContext->Draw(36, 0);

	//g_pImmediateContext->UpdateSubresource(g_pConstantBuffer1, 0, 0, &cb1_values, 0, 0);

	//g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer1);

	//g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer2, &stride, &offset);

	//// Select which primitive type to use //03-01
	//g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//// Draw the vertex buffer to the back buffer //03-01
	//g_pImmediateContext->Draw(36, 0);

	sphere->Draw(&view, &projection);
	sphere2->Draw(&view, &projection);

	// RENDER HERE

	g_p2DText->AddText("Text", -1, 1, 0.2f);

	g_p2DText->RenderText();

	// Display what has just been rendered
	g_pSwapChain->Present(0, 0);
}

void Update()
{
#pragma region old Code
	//if (GetKeyState('W') & 0x8000)
//{
//	vertices[0].Col.x = 1.0f;
//	vertices[1].Col.y = 0.0f;
//	vertices[2].Col.z = 1.0f;

//	/*vertices[0].pos.x += 0.1f;
//	vertices[1].pos.x += 0.1f;
//	vertices[2].pos.x += 0.1f;*/
//}
//if (GetKeyState('S') & 0x8000)
//{
//	vertices[0].Col.x = 0.0f;
//	vertices[1].Col.y = 1.0f;
//	vertices[2].Col.z = 0.0f;
//}
//if (GetKeyState('Q') & 0x8000)
//{
//	HRESULT hr = S_OK;
//	D3D11_BUFFER_DESC bufferDesc;
//	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
//	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;										//Allows use by CPU and GPU
//	bufferDesc.ByteWidth = sizeof(POS_COL_TEX__NORM_VERTEX) * 6;							//Set the total size of the buffer (in this case, 3 vertices)
//	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;							//Set the type of buffer to vertex buffer
//	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;							//Allow access by the CPU
//	hr = g_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &g_pVertexBuffer);		//Create the buffer

//	vertices[3].pos = XMFLOAT3(0.9f, 1.2f, 0.0f);
//	vertices[3].Col = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

//	vertices[4].pos = XMFLOAT3(0.9f, -0.6f, 0.0f);
//	vertices[4].Col = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

//	vertices[5].pos = XMFLOAT3(-0.9f, -0.6f, 0.0f);
//	vertices[5].Col = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
//}
//if (GetKeyState('R') & 0x8000)
//{
//	CONSTANT_BUFFER0 cb0_values;
//	if(redAmount != 3.0f)
//	{ 
//		redAmount += 0.1f;
//	}

//	cb0_values.scaleAmount = 0.6f;

//	cb0_values.RedAmount = redAmount;

//	g_pImmediateContext->UpdateSubresource(g_pConstantBuffer0, 0, 0, &cb0_values, 0, 0);

//	g_pImmediateContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer0);
//}  
#pragma endregion


	if (GetKeyState('W') & 0x8000)
	{
		camera->Forward(0.001);
	}
	if (GetKeyState('S') & 0x8000)
	{
		camera->Forward(-0.001);
	}
	if (GetKeyState('A') & 0x8000)
	{
		camera->Strafe(0.001);
	}
	if (GetKeyState('D') & 0x8000)
	{
		camera->Strafe(-0.001);
	}
	if (GetKeyState('Q') & 0x8000)
	{
		camera->Rotate(-0.01);
	}
	if (GetKeyState('E') & 0x8000)
	{
		camera->Rotate(0.01);
	}
	if (GetKeyState('R') & 0x8000)
	{
		camera->Pitch(0.01);
	}
	if (GetKeyState('F') & 0x8000)
	{
		camera->Pitch(-0.01);
	}
	if (GetKeyState('X') & 0x8000)
	{
		camera->Jump();
	}
	if (GetKeyState('1') & 0x8000)
	{
		g_directional_light_colour = XMVectorSet(0, -0.1f, 0.8f, 0);
	}
	if (GetKeyState('2') & 0x8000)
	{
		g_directional_light_colour = XMVectorSet(0, 0, 1.3f, 0);
	}
	if (GetKeyState('3') & 0x8000)
	{
		g_directional_light_colour = XMVectorSet(1, 0, 0, 0);
	}
	if (GetKeyState('I') & 0x8000)
	{
		g_directional_light_rotation = XMVectorSet(g_directional_light_rotation.x - 1, g_directional_light_rotation.y, g_directional_light_rotation.z, g_directional_light_rotation.w);
	}
	if (GetKeyState('O') & 0x8000)
	{
		g_directional_light_rotation = XMVectorSet(g_directional_light_rotation.x, g_directional_light_rotation.y + 1, g_directional_light_rotation.z, g_directional_light_rotation.w);
	}
	if (GetKeyState('P') & 0x8000)
	{
		g_directional_light_rotation = XMVectorSet(g_directional_light_rotation.x + 1, g_directional_light_rotation.y, g_directional_light_rotation.z, g_directional_light_rotation.w);
	}
	if (GetKeyState('L') & 0x8000)
	{
		g_directional_light_rotation = XMVectorSet(g_directional_light_rotation.x, g_directional_light_rotation.y - 1, g_directional_light_rotation.z, g_directional_light_rotation.w);
	}
	if (GetKeyState('N') & 0x8000)
	{
		sphere->incXPosition(0.01f);
	}
	if (GetKeyState('M') & 0x8000)
	{
		sphere->incYRotation(0.01f);
	}

	camera->Update();

	sphere->LookAt(sphere2->getPosition().x, sphere2->getPosition().z);
	sphere->MoveForward(-0.003f);
	if(sphere->CheckCollision(sphere2))
		sphere->MoveForward(0.003f);

	sphere2->LookAt(sphere->getPosition().x, sphere->getPosition().z);
	sphere2->MoveForward(-0.003f);
	if (sphere2->CheckCollision(sphere))
		sphere2->MoveForward(0.003f);

	//world2 *= XMMatrixTranslation(0, 0, 0.01);

	//Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;

	//Lock the buffer to allow writing
	g_pImmediateContext->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	//Copy the data
	memcpy(ms.pData, vertices, sizeof(vertices));

	//Unlock the buffer
	g_pImmediateContext->Unmap(g_pVertexBuffer, NULL);

	//Copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms2;

	//Lock the buffer to allow writing
	g_pImmediateContext->Map(g_pVertexBuffer2, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms2);

	//Copy the data
	memcpy(ms2.pData, box2, sizeof(box2));

	//Unlock the buffer
	g_pImmediateContext->Unmap(g_pVertexBuffer2, NULL);
}

void CreateMatrices()
{
	world = XMMatrixTranslation(0, 1, 2);
	world *= XMMatrixRotationX(XMConvertToRadians(70));
	world *= XMMatrixRotationY(XMConvertToRadians(70));
	world *= XMMatrixTranslation(0, 0, 15);

	world2 = XMMatrixTranslation(0, 1, 2);
	world2 *= XMMatrixRotationX(XMConvertToRadians(70));
	world2 *= XMMatrixTranslation(1, 0, 18);

	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(95.0), 640.0 / 480.0, 1.0, 100.0);
	view = camera->GetViewMatrix();
}