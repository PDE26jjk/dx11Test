#include "light.hlsli"

Texture2D gInputA;
Texture2D gInputB;
RWTexture2D<float4> gOutput;
[numthreads(16, 16, 1)]
void addCS( uint3 dispatchThreadID : SV_DispatchThreadID )
{
    gOutput[dispatchThreadID.xy] =
        gInputA[dispatchThreadID.xy] +
        gInputB[dispatchThreadID.xy];
}

technique11 add
{
    pass P0
    {
        SetVertexShader(NULL);
        SetGeometryShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, addCS()));
    }
}




