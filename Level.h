#pragma once
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT

#include <vector>
#include "Model.h"

class Level
{
	public:
		Level();
		Level(ID3D11Device* device, ID3D11DeviceContext* context);
		~Level();
		void startLevel();
		void drawLevel(XMMATRIX* view, XMMATRIX* projection);

	protected:
		vector<Model> objects;

		ID3D11Device*			m_pD3DDevice;
		ID3D11DeviceContext*	m_pImmediateContext;

		XMVECTOR g_directional_light_shines_from;
		XMVECTOR g_directional_light_colour;
		XMVECTOR g_ambient_light_colour;
		XMVECTOR g_directional_light_rotation;

		XMVECTOR g_point_light_colour;
		XMVECTOR g_point_light_position;
};