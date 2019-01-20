struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord : TEXCOORD;
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

Texture2D texture0;
SamplerState sampler0;

VOut VShader(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	output.position = mul(WVPMatrix, position);
	if (scale != 0)
	{
		output.position *= scale;
	}

	float4 lightvector = g_point_light_position - position;
	float point_amount = dot(normalize(lightvector), normal);

	point_amount = saturate(point_amount);

	float diffuse_amount = dot(directional_light_vector, normal);
	diffuse_amount = saturate(diffuse_amount);
	output.color = color + ambient_light_colour + (directional_light_colour * diffuse_amount) + (g_point_light_colour * point_amount);

	output.texcoord = texcoord;

	return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return color * texture0.Sample(sampler0, texcoord);
}
