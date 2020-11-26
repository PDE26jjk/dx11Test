
struct VertexIn
{
    float3 PosL  : POSITION;
    float3 NormalL : NORMAL;
};

struct VertexOut
{
    float4 PosH  : SV_POSITION;
    float3 NormalW : NORMAL;
};
VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    
    // Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

    return vout;
}


float4 PS(VertexOut pin) : SV_Target
{
    
     return float4(0.0f, 0.0f, 0.0f, 1.0f);
}

technique11 LightOnWaweTech
{

    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}