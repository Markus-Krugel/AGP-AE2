#include "Model.h"
#define _XM_NO_INTRINSICS_
#define _XM_NO_ALIGNMENT

Model::Model()
{
}

Model::Model(ID3D11Device * device, ID3D11DeviceContext * context)
{
	m_pD3DDevice = device;
	m_pImmediateContext = context;


	m_x = 0;
	m_y = 0;
	m_z = 0;
	m_xAngle = 0;
	m_yAngle = 0;
	m_zAngle = 0;
	m_scale = 1.0f;

	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 144;
	constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	HRESULT hr = S_OK;

	hr = m_pD3DDevice->CreateBuffer(&constant_buffer_desc, NULL, &m_pConstantBuffer);
}

HRESULT Model::LoadObjModel(char * filename)
{
	HRESULT hr = S_OK;

	m_pObject = new ObjFileModel(filename, m_pD3DDevice, m_pImmediateContext);

	if (m_pObject->filename == "FILE NOT LOADED") return S_FALSE;

	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelVS", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0)//Check for shader compilation error
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr))//Don't fail if error is just a warning
		{
			return hr;
		}
	}

	hr = D3DX11CompileFromFile("model_shaders.hlsl", 0, 0, "ModelPS", "ps_4_0", 0, 0, 0, &PS, &error, 0);

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
	hr = m_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &m_pVShader);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = m_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &m_pPShader);
	if (FAILED(hr))
	{
		return hr;
	}
	m_pImmediateContext->VSSetShader(m_pVShader, 0, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, 0, 0);
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		//Be very careful setting the correct dxgi format and D3D version
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		//NOTE the spelling of COLOR. Again, be careful setting the correct dxgi format (using A32) and correct D3D version
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = m_pD3DDevice->CreateInputLayout(iedesc, ARRAYSIZE(iedesc), VS->GetBufferPointer(), VS->GetBufferSize(), &m_pInputLayout);

	if (FAILED(hr))
	{
		return hr;
	}

	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	CalculateModelCentre();
	CalculateBoundingRadius();

	return S_OK;
}

void Model::Draw(XMMATRIX *view, XMMATRIX *projection)
{
	calculateWorld();

	MODEL_CONSTANT_BUFFER model_cb_values;
	model_cb_values.WorldViewProjection = world * (*view) * (*projection);

	XMMATRIX transpose = XMMatrixTranspose(world);
	

	model_cb_values.directional_light_colour = m_directional_light_colour;
	model_cb_values.ambient_light_colour = m_ambient_light_colour;
	model_cb_values.directional_light_vector = XMVector3Transform(m_directional_light_shines_from + m_directional_light_rotation, transpose);
	model_cb_values.directional_light_vector = XMVector3Normalize(model_cb_values.directional_light_vector);

	XMVECTOR determinant;
	XMMATRIX inverse;
	inverse = XMMatrixInverse(&determinant, world);

	m_point_light_position = XMVector3Transform(m_point_light_position, inverse);

	model_cb_values.point_light_colour = m_point_light_colour;
	model_cb_values.point_light_position = m_point_light_position;

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);

	m_pImmediateContext->VSSetShader(m_pVShader, NULL, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, NULL, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	// Anisotropic filtering enabling
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampler_desc.MaxAnisotropy = 16;
	m_pD3DDevice->CreateSamplerState(&sampler_desc, &m_pSampler);

	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture);

	m_pObject->Draw();
}

Model::~Model()
{
	m_pObject->~ObjFileModel();

	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pPShader) m_pPShader->Release();
	if (m_pVShader) m_pVShader->Release();
	if (m_pInputLayout) m_pInputLayout->Release();
	
	if(m_pTexture) m_pTexture->Release();
	if (m_pSampler) m_pSampler->Release();
}

void Model::AddTexture(char * filename)
{
	D3D11_TEXTURE2D_DESC desc{};
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;


	//m_pD3DDevice->CreateTexture2D(&desc, nullptr, Texture.GetAddressOf());
	//m_pD3DDevice->CreateShaderResourceView(Texture.Get(), &srvDesc, ShaderResource.GetAddressOf());
	//m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);
	//m_pImmediateContext->GenerateMips(ShaderResource.Get());

	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
}

void Model::CalculateModelCentre()
{
	float maxX = m_pObject->vertices[0].Pos.x;
	float maxY = m_pObject->vertices[0].Pos.y;
	float maxZ = m_pObject->vertices[0].Pos.z;

	float minX = m_pObject->vertices[0].Pos.x;
	float minY = m_pObject->vertices[0].Pos.y;
	float minZ = m_pObject->vertices[0].Pos.z;

	for (int i = 0; i < m_pObject->numverts; i++)
	{
		if (m_pObject->vertices[i].Pos.x > maxX)
			maxX = m_pObject->vertices[i].Pos.x;
		if (m_pObject->vertices[i].Pos.y > maxY)
			maxY = m_pObject->vertices[i].Pos.y;
		if (m_pObject->vertices[i].Pos.z > maxZ)
			maxZ = m_pObject->vertices[i].Pos.z;

		if (m_pObject->vertices[i].Pos.x < minX)
			minX = m_pObject->vertices[i].Pos.x;
		if (m_pObject->vertices[i].Pos.y < minY)
			minY = m_pObject->vertices[i].Pos.y;
		if (m_pObject->vertices[i].Pos.z < minZ)
			minZ = m_pObject->vertices[i].Pos.z;
	}

	m_bounding_centreX = (maxX - minX) / 2;
	m_bounding_centreY = (maxY - minY) / 2;
	m_bounding_centreZ = (maxZ - minZ) / 2;
}

void Model::CalculateBoundingRadius()
{
	float maxDist = 0;

	for (int i = 0; i < m_pObject->numverts; i++)
	{
		float distanceSquared = pow(m_pObject->vertices[i].Pos.x - m_bounding_centreX, 2) + pow(m_pObject->vertices[i].Pos.y - m_bounding_centreY, 2) + pow(m_pObject->vertices[i].Pos.z - m_bounding_centreZ, 2);
		if (distanceSquared > maxDist)
			maxDist = distanceSquared;
	}

	m_bounding_radius = maxDist;
}

XMVECTOR Model::GetBoundingSphereWorldSpacePosition()
{
	calculateWorld();

	XMVECTOR offset;

	offset = XMVectorSet(m_bounding_centreX, m_bounding_centreY, m_bounding_centreZ, 0.0);
	offset = XMVector3Transform(offset, world);

	return offset;
}

float Model::GetBoundingSphereRadius()
{
	return m_bounding_radius * m_scale;
}

boolean Model::CheckCollision(Model* model, xyz* direction)
{
	if (model == this)
		return false;
	else
	{
		XMVECTOR thisBounding = GetBoundingSphereWorldSpacePosition();
		XMVECTOR otherBounding = model->GetBoundingSphereWorldSpacePosition();

		double xThis = XMVectorGetX(thisBounding);
		double yThis = XMVectorGetY(thisBounding);
		double zThis = XMVectorGetZ(thisBounding);
		
		double xOther = XMVectorGetX(otherBounding);
		double yOther = XMVectorGetY(otherBounding);
		double zOther = XMVectorGetZ(otherBounding);

		float distanceSquared = pow(xThis - xOther, 2) + pow(yThis - yOther, 2) + pow(zThis - zOther, 2);

		if (distanceSquared <= pow(GetBoundingSphereRadius() + model->GetBoundingSphereRadius(), 2))
		{
			xyz* pointNow = &Maths::vectorToXYZ(&getPosition());

			xyz pointOld;
			pointOld.x = pointNow->x - direction->x;
			pointOld.y = pointNow->y - direction->y;
			pointOld.z = pointNow->z - direction->z;

			for (size_t i = 0; i < m_pObject->numverts; i+=3)
			{
				model->calculateWorld();

				XMVECTOR p1 = XMVectorSet(model->m_pObject->vertices[i].Pos.x,
					model->m_pObject->vertices[i].Pos.y,
					model->m_pObject->vertices[i].Pos.z, 0.0f);
				XMVECTOR p2 = XMVectorSet(model->m_pObject->vertices[i + 1].Pos.x,
					model->m_pObject->vertices[i + 1].Pos.y,
					model->m_pObject->vertices[i + 1].Pos.z, 0.0f);
				XMVECTOR p3 = XMVectorSet(model->m_pObject->vertices[i + 2].Pos.x,
					model->m_pObject->vertices[i + 2].Pos.y,
					model->m_pObject->vertices[i + 2].Pos.z, 0.0f);

				p1 = XMVector3Transform(p1, model->world);
				p2 = XMVector3Transform(p2, model->world);
				p3 = XMVector3Transform(p3, model->world);

				xyz* xyz1 = &Maths::vectorToXYZ(&p1);
				xyz* xyz2 = &Maths::vectorToXYZ(&p2);
				xyz* xyz3 = &Maths::vectorToXYZ(&p3);

				Plane plane = Maths::calculatePlane(xyz1, xyz2, xyz3);

				float planeValue1 = Maths::planeEquationForPoint(&plane, pointNow);
				float planeValue2 = Maths::planeEquationForPoint(&plane, &pointOld);

				if ((planeValue1 < 0 && planeValue2>0) || (planeValue1 > 0 && planeValue2 < 0))
				{
					xyz intersection = Maths::planeIntersection(&plane, &pointOld, pointNow);
					if (Maths::in_triangle(xyz1, xyz2, xyz3, pointNow))
						return true;
				}
			}
			//return true;
		}
		else
			return false;
	}
}

void Model::calculateWorld()
{
	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationRollPitchYaw(m_xAngle, m_yAngle, m_zAngle);
	world *= XMMatrixTranslation(m_x, m_y, m_z);
}


#pragma region get and set Position and Rotation Methods
void Model::setXPosition(float num)
{
	m_x = num;
}

void Model::setYPosition(float num)
{
	m_y = num;
}

void Model::setZPosition(float num)
{
	m_z = num;
}

void Model::setPosition(float x, float y, float z)
{
	m_x = x;
	m_y = y;
	m_z = z;
}

void Model::setPosition(XMVECTOR position)
{
	m_x = position.x;
	m_y = position.y;
	m_z = position.z;
}

float Model::getXPosition()
{
	return m_x;
}

float Model::getYPosition()
{
	return m_y;
}

float Model::getZPosition()
{
	return m_z;
}

XMVECTOR Model::getPosition()
{
	return XMVectorSet(m_x, m_y, m_z, 0);
}

void Model::setXRotation(float num)
{
	m_xAngle = num;
}

void Model::setYRotation(float num)
{
	m_yAngle = num;
}

void Model::setZRotation(float num)
{
	m_zAngle = num;
}

void Model::setRotation(float x, float y, float z)
{
	m_yAngle = y;
	m_zAngle = z;
	m_xAngle = x;
}

void Model::setRotation(XMVECTOR rotation)
{
	m_xAngle = rotation.x;
	m_yAngle = rotation.y;
	m_zAngle = rotation.z;
}

float Model::getXRotation()
{
	return m_xAngle;
}

float Model::getYRotation()
{
	return m_yAngle;
}

float Model::getZRotation()
{
	return m_zAngle;
}

XMVECTOR Model::getRotation()
{
	return XMVectorSet(m_xAngle, m_yAngle, m_zAngle, 0);
}

void Model::setScale(float num)
{
	m_scale = num;
}

float Model::getScale()
{
	return m_scale;
}
#pragma endregion

#pragma region increase methods

void Model::incXPosition(float num)
{
	m_x += num;
}

void Model::incYPosition(float num)
{
	m_y += num;
}

void Model::incZPosition(float num)
{
	m_z += num;
}

void Model::incPosition(float x, float y, float z)
{
	m_x += x;
	m_y += y;
	m_z += z;
}

void Model::incPosition(XMVECTOR position)
{
	m_x += position.x;
	m_y += position.y;
	m_z += position.z;
}

void Model::incXRotation(float num)
{
	m_xAngle += num;
}

void Model::incYRotation(float num)
{
	m_yAngle += num;
}

void Model::incZRotation(float num)
{
	m_z += num;
}

void Model::incRotation(float x, float y, float z)
{
	m_xAngle += x;
	m_yAngle += y;
	m_zAngle += z;
}

void Model::incRotation(XMVECTOR rotation)
{
	m_xAngle += rotation.x;
	m_yAngle += rotation.y;
	m_zAngle += rotation.z;
}

void Model::incScale(float num)
{
	m_scale += num;
}

#pragma endregion

#pragma region lightings set Methods

void Model::setAmbient_light_colour(XMVECTOR ambient_light_colour)
{
	m_ambient_light_colour = ambient_light_colour;
}

void Model::setDirectional_light_shines_from(XMVECTOR directional_light_shines_from)
{
	m_directional_light_shines_from = directional_light_shines_from;
}

void Model::setDirectional_light_colour(XMVECTOR directional_light_colour)
{
	m_directional_light_colour = directional_light_colour;
}

void Model::setDirectional_light_rotation(XMVECTOR directional_light_rotation)
{
	m_directional_light_rotation = directional_light_rotation;
}

void Model::setDirectionalLight(XMVECTOR directional_light_shines_from, XMVECTOR directional_light_colour, XMVECTOR directional_light_rotation)
{
	m_directional_light_shines_from = directional_light_shines_from;
	m_directional_light_colour = directional_light_colour;
	m_directional_light_rotation = directional_light_rotation;
}

void Model::setPoint_light_colour(XMVECTOR point_light_colour)
{
	m_point_light_colour = point_light_colour;
}

void Model::setPoint_light_position(XMVECTOR point_light_position)
{
	m_point_light_position = point_light_position;
}

void Model::setPointLight(XMVECTOR point_light_colour, XMVECTOR point_light_position)
{
	m_point_light_colour = point_light_colour;
	m_point_light_position = point_light_position;
}

#pragma endregion