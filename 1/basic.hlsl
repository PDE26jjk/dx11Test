#include "basic.hlsli"

Texture2D gBillboardMap;

// 阴影光源
float4 gShadowL;



float2 GetReflectUV(float3 PosL) {
    float4 PosH;
    PosH =  mul(float4(PosL, 1.0f), gReflectWorldViewProj);
    float4 PosD = PosH / PosH.a;
    float u = (PosD.x + 1.0f) / +2.0f;
    float v = (PosD.y - 1.0f) / -2.0f;
    float2 rUV = float2(u, v);
    return rUV;
}



struct BillboardVSIn
{
    float3 PosL : POSITION;
    float2 Size : SIZE;
};
struct BillboardVSOut
{
    float3 PosW : POSITION;
    float2 Size : SIZE;
};
BillboardVSOut BillboardVS(BillboardVSIn vin)
{
    lastTime = time;

    BillboardVSOut vout;
    vout.PosW = vin.PosL;
    vout.Size = vin.Size;
    return vout;
}
float2 gTexC[4] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(1.0f, 0.0f)
};
[maxvertexcount(4)]
void BillboardGS(point BillboardVSOut gin[1], inout TriangleStream<VertexOut> output) {
    /*float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 Tex : TEXCOORD;*/
    BillboardVSOut bpoint = gin[0];
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = gEyePosW - bpoint.PosW;
    look.y = 0.0f;
    look = normalize(look);
    float3 right = cross(up, look);
    float halfWidth = bpoint.Size.x / 2.0f;
    float Height = bpoint.Size.y ;
    float4 v[4];
    v[0] = float4(bpoint.PosW + halfWidth * right, 1.0f);
    v[1] = float4(bpoint.PosW + halfWidth * right + Height * up, 1.0f);
    v[2] = float4(bpoint.PosW - halfWidth * right, 1.0f);
    v[3] = float4(bpoint.PosW - halfWidth * right + Height * up, 1.0f);

    VertexOut gout;

    [unroll]
    for (int i = 0; i < 4; ++i) {
        gout.PosH = mul(v[i], gWorldViewProj);
        gout.PosW = v[i].xyz;
        gout.NormalW = look;
        gout.Tex = gTexC[i];
        //gout.PrimID = primID;
        output.Append(gout);

    }

}


VertexOut ShadowVS(VertexIn vin)
{
    lastTime = time;

    VertexOut vout;

    //Wave(vin.PosL.x, vin.PosL.z, vin.PosL.y, vin.NormalL);
    // 计算阴影矩阵
    float4 shadowPlane = float4(0.0f, 1.0f, 0.0f, 0.0f);
    float4x4 S = MatrixShadow(shadowPlane, gShadowL);
    vout.PosW = 0.0f;
    vout.NormalW = float3(0.0f, 1.0f, 0.0f);

    //vout.PosH = mul(float4(vin.PosL, 1.0f), mul(S,gWorldViewProj));
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

    // Output vertex attributes for interpolation across triangle.
    vout.Tex = 0.0f;
    //vout.Tex = vin.Tex;
    return vout;
}

VertexOut RefVS(VertexIn vin)
{
    lastTime = time;

    VertexOut vout;

    //Wave(vin.PosL.x, vin.PosL.z, vin.PosL.y, vin.NormalL);

    vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);

    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

    // Output vertex attributes for interpolation across triangle.
    vout.Tex = GetReflectUV(vin.PosL);
    //vout.Tex = vin.Tex;
    return vout;
}


float4 BillboardPS(VertexOut pin, uniform bool gUseTexure, uniform bool gUseFog = true) : SV_Target
{
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);
    if (gUseTexure)
    {
        // Sample texture.
        texColor = gBillboardMap.Sample(samAnisotropic, pin.Tex);
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
    //return litColor;
    //return float4(gMaterial.Diffuse.r, gMaterial.Diffuse.g, gMaterial.Diffuse.b,1.0f );
    return float4(texColor.r, texColor.g, texColor.b, texColor.a);
    //return float4(litColor.r, litColor.g, litColor.b,1.0f );
    //return float4(ambient.r, ambient.g, ambient.b,1.0f );
}

float4 RefPS(VertexOut pin, uniform bool gUseTexure, uniform bool gUseFog = true) : SV_Target
{
     // Default to multiplicative identity.
     float4 texColor = float4(0, 0, 0, 0);

     // Sample texture.
     int NumberOfSamples;
     // 纹理坐标转化
     float w,h,u,v;
     gReflectMap.GetDimensions(w, h, NumberOfSamples);
     
     u = lerp(0.0f, w, pin.Tex.x);
     v = lerp(0.0f, h, pin.Tex.y);
     for (int i = 0; i < NumberOfSamples; ++i) {
        texColor+= gReflectMap.sample[i][float2(u,v)];
     }
     texColor /= NumberOfSamples;

     // Start with a sum of zero.
     float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
     float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
     float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
     // Sum the light contribution from each light source.
     litPixel(pin,ambient, diffuse, spec);
     // light
     float4 litColor = texColor;

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
     //return float4(texColor.r, texColor.g, texColor.b,1.0f );
     //return float4(litColor.r, litColor.g, litColor.b,1.0f );
     //return float4(ambient.r, ambient.g, ambient.b,1.0f );
}

float4 ShadowPS(VertexOut pin, uniform bool gUseTexure, uniform bool gUseFog = true) : SV_Target
{
     // Start with a sum of zero.
     float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
     float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
     float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

     litPixel(pin,ambient, diffuse, spec);

     float4 litColor = ambient + diffuse + spec;
     litColor.rgb = 0.00f;
     float distToEye = distance(gEyePosW, pin.PosW);
     if (gUseFog)
     {
         float fogLerp = saturate((distToEye - gFogStart) / gFogRange);
         // Blend the fog color and the lit color. use alpha as amount
         //litColor = lerp(litColor, gFogColor, fogLerp * gFogColor.a);
     }
     // Common to take alpha from diffuse material.
     //litColor.xyz = 0.00f;
     litColor.a = gMaterial.Diffuse.a;
     //return litColor;
     //return float4(gMaterial.Diffuse.r, gMaterial.Diffuse.g, gMaterial.Diffuse.b,1.0f );
     //return float4(texColor.r, texColor.g, texColor.b,1.0f );
     //return float4(litColor.r, litColor.g, litColor.b,0.7f );
     //return float4(ambient.r, ambient.g, ambient.b,1.0f );
     return float4(0.0f, 0.0f, 0.0f,0.30f );
}


[maxvertexcount(9)]
void someGS(triangle VertexOut input[3], inout TriangleStream<VertexOut> output) {
    //float4 PosH : SV_POSITION;
    //float3 PosW : POSITION;
    //float3 NormalW : NORMAL;
    //float2 Tex : TEXCOORD;
    VertexOut vertexes[6];
    [unroll]
    for (unsigned int i = 0; i < 3; ++i) {
        vertexes[i] = input[i];
        vertexes[i + 3].PosH = (input[i].PosH + input[(i + 1) % 3].PosH) / 2.0f;
        vertexes[i + 3].PosW = (input[i].PosW + input[(i + 1) % 3].PosW) / 2.0f;
        vertexes[i + 3].NormalW = (input[i].NormalW + input[(i + 1) % 3].NormalW) / 2.0f;
        vertexes[i + 3].Tex = (input[i].Tex + input[(i + 1) % 3].Tex) / 2.0f;
    }
    [unroll]
    for (i = 0; i < 3; ++i) {
        output.Append(vertexes[i]);
        output.Append(vertexes[i+3]);
        output.Append(vertexes[(i+5)%3+3]);
        output.RestartStrip();
    }

}


[maxvertexcount(3)]
void someGS2(triangle VertexOut input[3], inout TriangleStream<VertexOut> output) {
    //float4 PosH : SV_POSITION;
    //float3 PosW : POSITION;
    //float3 NormalW : NORMAL;
    //float2 Tex : TEXCOORD;
    float3 NormalFaceW = 0;

    [unroll]
    for (int i = 0; i < 3; ++i) {
        NormalFaceW+=input[i].NormalW;      
    }
    NormalFaceW /= 3.0f;
    float littleTime = pow(2.71828f,time * 10.0f);

    [unroll]
    for (i = 0; i < 3; ++i) {
        input[i].PosW += NormalFaceW * littleTime;
        input[i].PosH = mul(float4(input[i].PosW,1.0f), gViewProj);
        output.Append(input[i]);
    }
}



technique11 Texture
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        //SetGeometryShader(CompileShader(gs_5_0, someGS2()));
        SetPixelShader(CompileShader(ps_5_0, PS(true)));
    }
}
technique11 Reflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, RefVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, RefPS(true)));
    }
}
technique11 ReflectNoMSAA
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, RefVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(true)));
    }
}

technique11 NOTexture
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        //SetGeometryShader(NULL);
        SetGeometryShader(CompileShader(gs_5_0, someGS2()));
        SetPixelShader(CompileShader(ps_5_0, PS(false)));
    }
}

technique11 NOFog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS(true,false)));
    }
}

technique11 Shadow
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, ShadowVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ShadowPS(true, false)));
    }
}
technique11 Billboard
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, BillboardVS()));
        SetGeometryShader(CompileShader(gs_5_0, BillboardGS()));
        SetPixelShader(CompileShader(ps_5_0, BillboardPS(true, false)));
    }
}

