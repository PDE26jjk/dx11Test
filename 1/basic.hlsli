#include "light.hlsli"
cbuffer cbPerFrame {
    float3 gEyePosW;
    float gFogStart;
    float gFogRange;
    float4 gFogColor;
};

cbuffer cbPerObject
{
    float time;
    float4x4 gWorld;
    float4x4 gWorldInvTranspose;
    float4x4 gWorldViewProj;
    float4x4 gViewProj;
    float4x4 gReflectWorldViewProj;
    float4x4 gTexTransform;
    Material gMaterial;
}

// 纹理
Texture2D gDiffuseMap;
Texture2DMS <float4, 4> gReflectMap;


struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 Tex : TEXCOORD;
};
struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 Tex : TEXCOORD;
};
// 记录时间
static float lastTime;
static const float dt = 0.5f;

// 采样器设置
SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;
    AddressU = BORDER;
    AddressV = BORDER;
};


// 计算灯光组
void litPixel(VertexOut pin, inout float4 ambient, inout float4 diffuse, inout float4 spec) {

    // Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW);
    float3 toEyeW = normalize(gEyePosW - pin.PosW);
    // Sum the light contribution from each light source.
    float4 A, D, S;
    for (int i = 0; i < num_DL; ++i) {
        ComputeDirectionalLight(gMaterial, DLs[i], pin.NormalW, toEyeW, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }
    for (int j = 0; j < num_PL; ++j) {
        ComputePointLight(gMaterial, PLs[j], pin.PosW, pin.NormalW, toEyeW, A, D, S);
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
}


VertexOut VS(VertexIn vin)
{
    lastTime = time;

    VertexOut vout;

    //Wave(vin.PosL.x, vin.PosL.z, vin.PosL.y, vin.NormalL);

    vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);

    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

    // Output vertex attributes for interpolation across triangle.
    vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
    //vout.Tex = vin.Tex;
    return vout;
}

float4 PS(VertexOut pin, uniform bool gUseTexure, uniform bool gUseFog = true) : SV_Target
{
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);
    if (gUseTexure)
    {
        // Sample texture.
        texColor = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
    }
    //clip(texColor.a - 0.05f);

    float4 litColor = texColor;

    // Start with a sum of zero.
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

    litPixel(pin, ambient, diffuse, spec);

    litColor = texColor * (ambient + diffuse) + spec;
    float distToEye = distance(gEyePosW, pin.PosW);
    if (gUseFog)
    {
        float fogLerp = saturate((distToEye - gFogStart) / gFogRange);
        // Blend the fog color and the lit color. use alpha as amount
        litColor = lerp(litColor, gFogColor, fogLerp * gFogColor.a);
    }
    // Common to take alpha from diffuse material.
    litColor.a = texColor.a * gMaterial.Diffuse.a;
    return litColor;
    //return float4(gMaterial.Diffuse.r, gMaterial.Diffuse.g, gMaterial.Diffuse.b,1.0f );
    //return float4(texColor.r, texColor.g, texColor.b, texColor.a);
    //return float4(litColor.r, litColor.g, litColor.b,1.0f );
    //return float4(ambient.r, ambient.g, ambient.b,1.0f );
}
