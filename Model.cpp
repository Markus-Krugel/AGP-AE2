#include "Model.h"

Model::Model(ID3D11Device * device, ID3D11DeviceContext * context)
{
	m_pD3DDevice = device;
	m_pImmediateContext = context;


	m_x = 0;
	m_y = 0;
	m_z = 10.0f;
	m_xAngle = 0;
	m_yAngle = 0;
	m_zAngle = 0;
	m_scale = 1.0f;

	D3D11_BUFFER_DESC constant_buffer_desc;
	ZeroMemory(&constant_buffer_desc, sizeof(constant_buffer_desc));

	constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	constant_buffer_desc.ByteWidth = 160;
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

	return S_OK;
}

void Model::Draw(XMMATRIX *view, XMMATRIX *projection)
{
	XMMATRIX world;
	world = XMMatrixScaling(m_scale, m_scale, m_scale);
	world *= XMMatrixRotationRollPitchYaw(m_xAngle, m_yAngle, m_zAngle);
	world *= XMMatrixTranslation(m_x, m_y, m_z);

	MODEL_CONSTANT_BUFFER model_cb_values;
	model_cb_values.WorldViewProjection = world * (*view) * (*projection);

	m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, 0, &model_cb_values, 0, 0);

	m_pImmediateContext->VSSetShader(m_pVShader, NULL, 0);
	m_pImmediateContext->PSSetShader(m_pPShader, NULL, 0);
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);

	m_pImmediateContext->PSSetSamplers(0, 1, &m_pSampler);
	m_pImmediateContext->PSSetShaderResources(0, 1, &m_pTexture);

	m_pObject->Draw();
}

Model::~Model()
{
	delete m_pObject;

	if (m_pConstantBuffer) m_pConstantBuffer->Release();
	if (m_pPShader) m_pPShader->Release();
	if (m_pVShader) m_pVShader->Release();
	if (m_pInputLayout) m_pInputLayout->Release();

	if(m_pTexture) m_pTexture->Release();
	if (m_pSampler) m_pSampler->Release();
}

void Model::AddTexture(char * filename)
{
	D3DX11CreateShaderResourceViewFromFile(m_pD3DDevice, "Assets/texture.jpg", NULL, NULL, &m_pTexture, NULL);

	D3D11_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
}

#pragma region get and set Methods
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