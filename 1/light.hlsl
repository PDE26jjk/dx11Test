#include "light.hlsli"

cbuffer cbPerFrame
{
    //DirectionalLight gDirLight;
    //PointLight gPointLight;
    //SpotLight gSpotLight;
    float3 gEyePosW;
};
cbuffer cbPerObject
{
    float time;
    float4x4 gWorld;
    float4x4 gWorldInvTranspose;
    float4x4 gWorldViewProj;
    Material gMaterial;
}
// 记录时间
static float lastTime;
static const float dt = 0.5f;



struct VertexIn
{
    float3 PosL  : POSITION;
    float3 NormalL : NORMAL;
};

struct VertexOut
{
    float3 PosW : POSITION;
    float4 PosH  : SV_POSITION;
    float3 NormalW : NORMAL;
};

 // 根据时间模拟波浪
void Wave(
    float x,
    float z,
    out float y,
    out float3 n
) {

    y = 0.1f * (z * sin(0.2f * x + time / 1.0f) + x * cos(0.2f * z + time / 1.0f));
    // Transform to world space.
    // 计算法线
    n = float3(
        0.02 * z * cos(0.2 * x + time / 1.0f) + 0.1 * cos(0.2f * z + time / 1.0f),
        1.0f,
        0.1 * sin(0.2 * x + time / 1.0f) + 0.02 * x * sin(0.2f * z + time / 1.0f)
        );
    n /= length(n);
}


VertexOut WaveVS(VertexIn vin)
{
    lastTime = time;

    VertexOut vout;

    //Wave(vin.PosL.x, vin.PosL.z, vin.PosL.y, vin.NormalL);

    vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);

    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

    // Just pass vertex color into the pixel shader.

    return vout;
}

float4 LightPS(VertexOut pin) : SV_Target
{
    // Interpolating normal can unnormalize it, so normalize it.
     pin.NormalW = normalize(pin.NormalW);
     float3 toEyeW = normalize(gEyePosW - pin.PosW);
     // Start with a sum of zero.
     float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
     float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
     float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
     // Sum the light contribution from each light source.
     float4 A, D, S;

     for (int i = 0; i < num_DL; ++i) {
         ComputeDirectionalLight(gMaterial, DLs[i],pin.NormalW, toEyeW, A, D, S);
         ambient += A;
         diffuse += D;
         spec += S;
     }
     for (int j = 0; j < num_PL; ++j) {
         ComputePointLight(gMaterial, PLs[j],pin.PosW, pin.NormalW, toEyeW, A, D, S);
         ambient += A;
         diffuse += D;
         spec += S;
     }
     for (int k = 0; k < num_SL; ++k) {
         ComputeSpotLight(gMaterial, SLs[k], pin.PosW, pin.NormalW, toEyeW, A, D, S);
         ambient += A;
         diffuse += D;
         spec += S;
     }
     float4 litColor = clamp(ambient + diffuse + spec,0.0f,1.0f);
     // Common to take alpha from diffuse material.
     litColor.a = gMaterial.Diffuse.a;
     return litColor;
     //return float4(0.0f, 0.0f, 0.0f, 1.0f);
}

float4 PS(VertexOut pin) : SV_Target{

    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}

RasterizerState WireframeRS
{
    FillMode = Wireframe;
    CullMode = Back;
    FrontCounterClockwise = false;
    // Default values used for any properties we do not set.
};

technique11 LightOnWaweTech

{   pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, WaveVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, LightPS()));
    }
    /*pass P1
    {
        SetVertexShader(CompileShader(vs_5_0, WaveVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
        SetRasterizerState(WireframeRS);
    }*/
    
    

}