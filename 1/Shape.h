#pragma once
#include "h.h"
#include "vertex.h"
#include "light.h"
//#include "BasicEffect.h"

namespace wrl = Microsoft::WRL;
class BasicEffect;
class PDEeffect;
enum class BlendType {
	None,
	Add,
	Subtract,
	Multiply,
	Transparency,
};

class PDEshape {
	friend class Graphics;
	friend class GeometryGenerator;
	friend class BasicEffect;
public:

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};
public:
	PDEshape(MeshData& meshData);
	PDEshape(const PDEshape& sold) = default;
	PDEshape& operator=(const PDEshape& sold) = default;
	void Move(float x,float y,float z);
	void Scale(float x,float y,float z);
	void Rotate(XMVECTOR A,float angle);

	void SetShadow(bool HasShadow) { this->HasShadow = HasShadow; }
	void MoveTex(float u, float v);
	void ScaleTex(float x, float y);
	void RotateTex(XMVECTOR A, float angle);
	void ResetTex();

	void SetMaterial(Material Mat) { this->Mat = Mat; }
	void SetBlendType(BlendType blendType) { this->blendType = blendType; }
	void SetMatOpaque(float opaque);
	void SetTexture(LPCTSTR file) { this->Texture = file; }
	void SetTextureView(ID3D11ShaderResourceView* pSRV) { 
		this->ShaderResourceView = wrl::ComPtr<ID3D11ShaderResourceView>(pSRV);
	}

	PDEeffect* GetEffect() { return mEffect; }
	virtual void init();

	static XMMATRIX InverseTranspose(CXMMATRIX M);
protected:
	PDEshape() = default;
	// ������
	MeshData meshData;

	wrl::ComPtr<ID3D11ShaderResourceView> ShaderResourceView;
	// ��ʼ��Ϊȫ���䣬SpecPower=16
	void initMat();
	// ȫ�־���
	XMFLOAT4X4 W;
	// ��ͼת������
	XMFLOAT4X4 TexW;
	// ����
	Material Mat;
	// ���ѡ��
	BlendType blendType;

	// ��ͼ�ļ�
	LPCTSTR Texture = nullptr;

	// �Ƿ�����Ӱ
	bool HasShadow = true;

	// ����Ч��
	PDEeffect* mEffect;
};