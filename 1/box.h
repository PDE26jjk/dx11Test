#pragma once
#include "h.h"

// ����һ��ֻ����ɫ��λ�õĶ�����Ϣ
using namespace DirectX;
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Color;
};

// �̳����ϸ���D3DApp��
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

	// ����������꣨�������꣬theta��x����xz�棬phi��y�𡣣�
	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
};
