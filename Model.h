#pragma once
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT

#include "objfilemodel.h"
#include "maths.h"

template <size_t align> class AlignedAllocPolicy
{
public:
	void* operator new(std::size_t size)
	{
		return _aligned_malloc(size, align);
	}

	void operator delete(void* mem)
	{
		_aligned_free(mem);
	}
};

struct MODEL_CONSTANT_BUFFER
{
	XMMATRIX WorldViewProjection;
	XMVECTOR directional_light_vector;
	XMVECTOR directional_light_colour;
	XMVECTOR ambient_light_colour;
	XMVECTOR point_light_colour;
	XMVECTOR point_light_position;
};

_declspec(align(16)) class Model : public AlignedAllocPolicy<16>
{
	public:
		Model();
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
		void setScale(float scaleX, float scaleY, float scaleZ);
		float getXPosition();
		float getYPosition();
		float getZPosition();
		XMVECTOR getPosition();
		float getXRotation();
		float getYRotation();
		float getZRotation();
		XMVECTOR getRotation();
		xyz getScale();

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

		void setAmbient_light_colour(XMVECTOR ambient_light_colour);

		void setDirectional_light_shines_from(XMVECTOR directional_light_shines_from);
		void setDirectional_light_colour(XMVECTOR directional_light_colour);
		void setDirectional_light_rotation(XMVECTOR directional_light_rotation);
		void setDirectionalLight(XMVECTOR directional_light_shines_from, XMVECTOR directional_light_colour, XMVECTOR directional_light_rotation);

		void setPoint_light_colour(XMVECTOR point_light_colour);
		void setPoint_light_position(XMVECTOR point_light_position);
		void setPointLight(XMVECTOR point_light_colour, XMVECTOR point_light_position);

		float GetBoundingSphereRadius();
		boolean CheckCollision(Model* model, xyz* direction);

		void calculateWorld();

	protected:
		ID3D11Device*			m_pD3DDevice;
		ID3D11DeviceContext*	m_pImmediateContext;
	
		ObjFileModel*			m_pObject;
		ID3D11VertexShader*		m_pVShader;
		ID3D11PixelShader*		m_pPShader;
		ID3D11InputLayout*		m_pInputLayout;
		ID3D11Buffer*			m_pConstantBuffer;

	
		float					m_x, m_y, m_z, directionX, directionZ;
		float					m_xAngle, m_yAngle, m_zAngle;
		float					m_scaleX, m_scaleY, m_scaleZ;

		ID3D11ShaderResourceView*	m_pTexture;
		ID3D11SamplerState*			m_pSampler;
		D3D11_SAMPLER_DESC			sampler_desc;

		XMVECTOR				m_directional_light_shines_from;
		XMVECTOR				m_directional_light_colour;
		XMVECTOR				m_ambient_light_colour;
		XMVECTOR				m_directional_light_rotation;
								
		XMVECTOR				m_point_light_colour;
		XMVECTOR				m_point_light_position;

		XMMATRIX				world;

		float					m_bounding_centreX;
		float					m_bounding_centreY;
		float					m_bounding_centreZ;
		float					m_bounding_radius;

		void					CalculateModelCentre();

		void CalculateBoundingRadius();

		XMVECTOR GetBoundingSphereWorldSpacePosition();

};
