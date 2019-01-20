struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float3 texcoord : TEXCOORD;
};

cbuffer CBuffer0
{
	matrix WVPMatrix;
	float4 directional_light_vector;
	float4 directional_light_colour;
	float4 ambient_light_colour;
	float scale;
	float4 g_point_light_colour;
	float4 g_point_light_position;
	float3 packing;
};

TextureCube texture0;
SamplerState sampler0;

VOut VShader(float4 position : POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	output.position = mul(WVPMatrix, position);
	if (scale != 0)
	{
		output.position *= scale;
	}

	output.color = color ;

	output.texcoord = position.xyz;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD) : SV_TARGET
{
	return color * texture0.Sample(sampler0, texcoord);
}
