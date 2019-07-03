#pragma once
#include "objfilemodel.h"
struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection;
};
class Model 
{
	public:
		Model(ID3D11Device* device, ID3D11DeviceContext* context);
		HRESULT LoadObjModel(char* filename);
		void Draw(XMMATRIX* view, XMMATRIX* projection);
		~Model();

		void setXPosition(float num);
		void setYPosition(float num);
		void setZPosition(float num);
		void setPosition(float x,float y, float z);
		void setPosition(XMVECTOR position);
		void setXRotation(float num);
		void setYRotation(float num);
		void setZRotation(float num);
		void setRotation(float x, float y, float z);
		void setRotation(XMVECTOR rotation);
		void setScale(float scale);
		float getXPosition();
		float getYPosition();
		float getZPosition();
		XMVECTOR getPosition();
		float getXRotation();
		float getYRotation();
		float getZRotation();
		XMVECTOR getRotation();
		float getScale();

		void incXPosition(float num);
		void incYPosition(float num);
		void incZPosition(float num);
		void incPosition(float x, float y, float z);
		void incPosition(XMVECTOR position);
		void incXRotation(float num);
		void incYRotation(float num);
		void incZRotation(float num);
		void incRotation(float x, float y, float z);
		void incRotation(XMVECTOR position);
		void incScale(float num);
		
		void AddTexture(char* filename);

	private:
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
};
