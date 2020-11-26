#pragma once
#include "h.h"

// 定义一个只有颜色和位置的顶点信息
using namespace DirectX;
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Color;
};

// 继承书上给的D3DApp类
class Box :
{
public:
	Box();
	~Box();


private:
	void BuildGeometryBuffers();
	void BuildFX();
	void BuildVertexLayout();

private:
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	ID3DX11Effect* mFX;
	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectMatrixVariable* mfxWorldViewProj;

	ID3D11InputLayout* mInputLayout;

	XMFLOAT4X4 mWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	// 摄像机球坐标（左手坐标，theta从x起沿xz面，phi从y起。）
	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};
