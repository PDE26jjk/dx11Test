#include "basic.hlsli"

//龙书p384
//
//VERSION 2: Using SampleLevel and texture coordinates.
//
cbuffer cbUpdateSettings
{
	float gWaveConstants[3];
	float2 gDisplacementMapTexelSize = float2(1.0f, 1.0f);
	float gGridSpatialStep = 5.0f;
};
SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};
SamplerState samDisplacement
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = BORDER;
	AddressV = BORDER;
};
Texture2D gPrevSolInput;
Texture2D gCurrSolInput;
RWTexture2D<float> gNextSolOutput;

#define RAND_MAX 0x7fff

//伪随机数算法 https://blog.csdn.net/pizi0475/article/details/48518665
#define cMult 0.0001002707309736288
#define aSubtract 0.2727272727272727
float randGrieu(float4 t)
{
	float a = t.x + t.z * cMult + aSubtract - floor(t.x);
	a *= a;
	float b = t.y + a;
	b -= floor(b);
	float c = t.z + b;
	c -= floor(c);
	float d = c;
	a += c * cMult + aSubtract - floor(a);
	a *= a;
	b += a;
	b -= floor(b);
	c += b;
	c -= floor(c);
	return (a+b+c+d)/4.0f;
}

[numthreads(16, 16, 1)]
void WaveCS(int3 dispatchThreadID : SV_DispatchThreadID)
{
	// Equivalently using SampleLevel() instead of operator [].
	int x = dispatchThreadID.x;
	int y = dispatchThreadID.y;
	float2 c = float2(x, y) / 512.0f;
	float2 t = float2(x, y - 1) / 512.0f;
	float2 b = float2(x, y + 1) / 512.0f;
	float2 l = float2(x - 1, y) / 512.0f;
	float2 r = float2(x + 1, y) / 512.0f; 
	//gNextSolOutput[int2(x, y)] =
	//	gWaveConstants[0] * gPrevSolInput.SampleLevel(samPoint, c, 0.0f).r +
	//	gWaveConstants[1] * gCurrSolInput.SampleLevel(samPoint, c, 0.0f).r +
	//	gWaveConstants[2] * (
	//		gCurrSolInput.SampleLevel(samPoint, b, 0.0f).r +
	//		gCurrSolInput.SampleLevel(samPoint, t, 0.0f).r +
	//		gCurrSolInput.SampleLevel(samPoint, r, 0.0f).r +
	//		gCurrSolInput.SampleLevel(samPoint, l, 0.0f).r);
	//gNextSolOutput[int2(x, y)] = 0.5f;
	

	// 扰乱
	/*if (time - t_base > 0.025f)
		return;*/
	//if (time - t_base < 0.25f) {

	//float now = time;
	//GroupMemoryBarrierWithGroupSync();
	float t_base = 500.0f;
	if (time - t_base > 0.0f) {
		t_base += 500.0f;
		float magnitude = frac(randGrieu(float4(time - t_base,c,lastTime - x - y)));
		float q = abs(frac(randGrieu(float4(time,t,lastTime - y))));
		float w = abs(frac(randGrieu(float4(time,b,lastTime - x))));
		//float magnitude = 0.5f;
		x = 511 * q;
		y = 511 * w;
		float halfMag = 0.5f * magnitude;

		 //Disturb the ijth vertex height and its neighbors.
		gNextSolOutput[int2(x,y)] = magnitude;
		gNextSolOutput[int2(x, y+1)] = halfMag;
		gNextSolOutput[int2(x, y-1)] = halfMag;
		gNextSolOutput[int2(x+1, y)] = halfMag;
		gNextSolOutput[int2(x-1, y)] = halfMag;
	}
		
	
}




VertexOut WaveVS(VertexIn vin)
{
	VertexOut vout;

	Texture2D gDisplacementMap = gCurrSolInput;

	// Sample the displacement map using non-transformed
	// [0,1]^2 tex-coords.
	vin.PosL.y = gDisplacementMap.SampleLevel(
		samDisplacement, vin.Tex, 0.0f).r;
	// Estimate normal using finite difference.
	float du = gDisplacementMapTexelSize.x;
	float dv = gDisplacementMapTexelSize.y;
	float l = gDisplacementMap.SampleLevel(
		samDisplacement, vin.Tex - float2(du, 0.0f), 0.0f).r;
	float r = gDisplacementMap.SampleLevel(
		samDisplacement, vin.Tex + float2(du, 0.0f), 0.0f).r;
	float t = gDisplacementMap.SampleLevel(
		samDisplacement, vin.Tex - float2(0.0f, dv), 0.0f).r;
	float b = gDisplacementMap.SampleLevel(
		samDisplacement, vin.Tex + float2(0.0f, dv), 0.0f).r;
	vin.NormalL = normalize(float3(-r + l, 2.0f * gGridSpatialStep, b - t));
	// Transform to world space space.
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
	return vout;
}
technique11 WaveTex {
	pass P0
	{
		SetVertexShader(NULL);
		SetGeometryShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, WaveCS()));
	}
};
technique11 DrawWave {
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, WaveVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS(true, false)));
	}
};