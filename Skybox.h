#pragma once
#define _XM_NO_INTRINSICS_
#define _XM_NO_ALIGNMENT
#include "objfilemodel.h"

struct MODEL_CONSTANT_BUFFERSkybox
{
	XMMATRIX WorldViewProjection;
	XMVECTOR directional_light_vector;
	XMVECTOR directional_light_colour;
	XMVECTOR ambient_light_colour;
	XMVECTOR point_light_colour;
	XMVECTOR point_light_position;
};

class Skybox 
{
	public:
		Skybox(ID3D11Device* device, ID3D11DeviceContext* context, XMVECTOR cameraPosition);
		HRESULT LoadObjModel(char* filename);
		void Draw(XMMATRIX* view, XMMATRIX* projection);
		~Skybox();

		void AddTexture(char* filename);

	protected:
		ID3D11Device*			m_pD3DDevice;
		ID3D11DeviceContext*	m_pImmediateContext;

		ObjFileModel*			m_pObject;
		ID3D11VertexShader*		m_pVShader;
		ID3D11PixelShader*		m_pPShader;
		ID3D11InputLayout*		m_pInputLayout;
		ID3D11Buffer*			m_pConstantBuffer;


		float					m_x, m_y, m_z;
		float					m_xAngle, m_yAngle, m_zAngle;
		float					m_scale;

		ID3D11ShaderResourceView*	m_pTexture;
		ID3D11SamplerState*			m_pSampler;

		ID3D11RasterizerState*		g_pRasterSolid = 0;
		ID3D11RasterizerState*		g_pRasterSkybox = 0;
		ID3D11DepthStencilState*	g_pDepthWriteSolid = 0;
		ID3D11DepthStencilState*	g_pDepthWriteSkybox = 0;
};