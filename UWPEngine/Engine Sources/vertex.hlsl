#pragma pack_matrix( row_major )

cbuffer ConstantBuffer:register(b0)
{
	float4x4 gWorldMatrix;
	float4x4 gViewMatrix;
	float4x4 gProjectionMatrix;
};

float4 main(float3 pos:POSITION) : SV_POSITION
{
	float4 vOut;
	float4 temp = float4(pos, 1);

	vOut = mul(temp, gWorldMatrix);
	vOut = mul(vOut, gViewMatrix);
	vOut = mul(vOut, gProjectionMatrix);

	return vOut;
}