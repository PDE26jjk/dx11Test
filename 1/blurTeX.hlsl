
#define N 256

float gWeights[11] = { 0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f};
static const int gBlurRadius = 5;
Texture2D gInput;
RWTexture2D<float4> gOutput;

// 边缘镜像的缓存数组[R ...3 2 1 | 0 1 2 3 4 ..N-3 N-2 N-1| N-2 N-3...N-R] 
groupshared float4 gShared[N + 2 * gBlurRadius];

[numthreads(N,1,1)]
void HorzBlurCS(int3 dtID : SV_DispatchThreadID,
				int3 gtID : SV_GroupThreadID)
{
	// 最左边界的取样，取镜像
	if (gtID.x <= gBlurRadius) {
		if(dtID.x <= gBlurRadius)
			gShared[gBlurRadius - dtID.x] = gInput[int2(dtID.x,dtID.y)];
			//gShared[gBlurRadius - dtID.x] = float4(0.0f, 0.0f, 0.0f, 0.0f);
		else
			gShared[gtID.x] = gInput[int2(dtID.x- gBlurRadius,dtID.y)];
			//gShared[gtID.x] = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	// 最右边界的取样，同上
	if (gtID.x >= N-1-gBlurRadius) {
		if (dtID.x >= int(gInput.Length.x) - 1 -gBlurRadius)
			gShared[2*(N-1) + gBlurRadius - dtID.x] = gInput[int2(dtID.x, dtID.y)];
		else
			gShared[gtID.x+2*gBlurRadius] = gInput[int2(dtID.x + gBlurRadius, dtID.y)];
			//gShared[gtID.x+2*gBlurRadius] = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	// 中间缓冲区采样
	gShared[gtID.x + gBlurRadius] = gInput[min(dtID.xy, gInput.Length.xy - 1)];
	// 等待同步
	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll]
	for (int i = -gBlurRadius; i <= gBlurRadius; ++i) {
		blurColor += gShared[gtID.x+gBlurRadius + i]* gWeights[gBlurRadius + i];
	}
	gOutput[int2(dtID.x, dtID.y)] = blurColor;
}

[numthreads(1, N, 1)]
void VertBlurCS(int3 dtID : SV_DispatchThreadID,
	int3 gtID : SV_GroupThreadID)
{
	
	// 最上边界的取样，取镜像
	if (gtID.y <= gBlurRadius) {
		if (dtID.y <= gBlurRadius)
			gShared[gBlurRadius - dtID.y] = gInput[int2(dtID.x, dtID.y)];
		else
			gShared[gtID.y] = gInput[int2(dtID.x, dtID.y - gBlurRadius)];
	}

	// 最下边界的取样，同上
	if (gtID.y >= N -1 - gBlurRadius) {
		if (dtID.y >= int(gInput.Length.y) - 1 - gBlurRadius)
			gShared[2 * (N - 1) + gBlurRadius - dtID.y] = gInput[int2(dtID.x, dtID.y)];
		else
			gShared[gtID.y + 2 * gBlurRadius] = gInput[int2(dtID.x, dtID.y + gBlurRadius)];
	}
	// 中间缓冲区采样
	gShared[gtID.y + gBlurRadius] = gInput[min(dtID.xy, gInput.Length.xy - 1)];
	// 等待同步
	GroupMemoryBarrierWithGroupSync();

	float4 blurColor = float4(0, 0, 0, 0);

	[unroll]
	for (int i = -gBlurRadius; i <= gBlurRadius; ++i) {
		blurColor += gShared[gtID.y + gBlurRadius + i] * gWeights[gBlurRadius + i];
	}
	gOutput[int2(dtID.x, dtID.y)] = blurColor;
}

technique11 HorzBlur
{
	pass P0
	{
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, HorzBlurCS()));
	}
}
technique11 VertBlur
{
	pass P0
	{
		SetVertexShader(NULL);
		SetPixelShader(NULL);
		SetComputeShader(CompileShader(cs_5_0, VertBlurCS()));
	}
}