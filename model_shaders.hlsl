cbuffer CB0
{
	matrix WVPMatrix;
	float4 directional_light_vector;
	float4 directional_light_colour;
	float4 ambient_light_colour;
	float4 point_light_colour;
	float4 point_light_position;
};

Texture2D texture0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

VOut ModelVS(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	float4 default_colour = { 1,1,1,1 };
	output.position = mul(WVPMatrix, position);
	output.texcoord = texcoord;

	float4 lightvector = point_light_position - position;
	float point_amount = dot(normalize(lightvector), normal);

	point_amount = saturate(point_amount);

	float diffuse_amount = dot(directional_light_vector, normal);
	diffuse_amount = saturate(diffuse_amount);
	output.color = color + ambient_light_colour + (directional_light_colour * diffuse_amount) + (point_light_colour * point_amount);

	return output;
}

float4 ModelPS(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return texture0.Sample(sampler0, texcoord) * color;
}