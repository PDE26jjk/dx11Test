#ifndef BASICEFFECT
#define BASICEFFECT
#include "h.h"
#include "effect.h"
#include "light.h"
#include "Graphics.h"
#include "TopoLayout.h"
#include "ReflectivePlane.h"


namespace wrl = Microsoft::WRL;
class ReflectivePlane;
class PDEshape;

class PDEeffect {
	friend class Graphics;
public:
	PDEeffect(const char* FXfilename, Graphics* mGfx);

	PDEeffect(PDEeffect&) = delete;
	PDEeffect& operator=(PDEeffect&) = delete;

	ID3DX11EffectTechnique* Tech() { return mTech.Get(); }
	virtual void Draw() = 0;
	void Bind();
	void CreateState();
	void CreateAllRasterizerState();
	void CreateAllBlendState();
	void CreateAllDepthStencilState();

	ID3DX11EffectTechnique* GetDrawTech() { return this->mTech.Get(); }

	ID3DX11Effect* GetFX(){ return mFX.Get(); }
	Graphics* GetGfx(){ return mGfx; }

protected:

	virtual void LoadFx()=0;
	virtual void SetTopoLayout() = 0;
	TopoLayout mTopoLayout;
	PDEeffect();
	wrl::ComPtr<ID3D11Device> pDevice;
	wrl::ComPtr<ID3D11DeviceContext> pContext;

	Graphics* mGfx;
	wrl::ComPtr<ID3DX11Effect> mFX;
	wrl::ComPtr<ID3DX11EffectTechnique> mTech;
	D3DX11_TECHNIQUE_DESC techDesc;

	// ���㻺��
	wrl::ComPtr<ID3D11Buffer> mVB;
	// ��������
	wrl::ComPtr<ID3D11Buffer> mIB;
	UINT mVBstride; // ���㻺�岽��
	UINT mVBoffset; // ���㻺��ƫ��

	// ����դ�񻯷�ʽ������Ⱦ���߿���Ⱦ�Ϳ�������
	static wrl::ComPtr<ID3D11RasterizerState> mWIREFRAME;
	static wrl::ComPtr<ID3D11RasterizerState> mSOLID;
	static wrl::ComPtr<ID3D11RasterizerState> mBACK;

	// ����趨
	static wrl::ComPtr<ID3D11BlendState> mBlendStateSRC;
	static wrl::ComPtr<ID3D11BlendState> mBlendStateAdd;
	static wrl::ComPtr<ID3D11BlendState> mBlendStateSubtract;
	static wrl::ComPtr<ID3D11BlendState> mBlendStateMultiply;
	static wrl::ComPtr<ID3D11BlendState> mBlendStateTransparency;

	// ���ģ���趨
	static wrl::ComPtr<ID3D11DepthStencilState> mNoDoubleBlendDSS;

	static bool mIsLoadState;

};

// ����ģʽ
class BasicEffect : public PDEeffect {

public:
	static BasicEffect* get_instance(Graphics* gfx) {
		if (this_ptr == nullptr) {
			this_ptr = new BasicEffect(gfx);
		}
		return this_ptr;
	}
	static BasicEffect* get_instance() {
		return this_ptr;
	}
	~BasicEffect() { if (this_ptr != nullptr) { delete this_ptr; this_ptr = nullptr; } }
	
	void init();
	BasicEffect() = delete;
	// ͨ�� PDEeffect �̳�
	virtual void Draw() override;
	void DrawShapes(ID3D11RenderTargetView* renderTarget, Camera& camera);
	void DrawMirrors(ID3D11RenderTargetView* target, Camera& camera);

	std::vector<UINT>& SortShapes(Camera& camera) const;
	
	void AddShape(PDEshape* pShape);
	void AddShapes(std::vector<PDEshape*> pShape);
	
	bool AddLight(Light light);

	struct Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};
	std::vector<PDEshape*>& GetShapes() { return pShapes; }
protected:
	virtual void LoadFx() override;
	virtual void SetTopoLayout() override;
private:
	void LoadMirrors(const std::vector<ReflectivePlane*>& mirrors);
	void LoadShapes();
	BasicEffect(Graphics* gfx);
	// ����ָ��
	static BasicEffect* this_ptr;
	//����ͼ��
	std::vector<PDEshape*> pShapes;
	bool mIsLoadShapes;

	/***************����**********************/
	wrl::ComPtr<ID3DX11EffectTechnique> mTechTexture;
	wrl::ComPtr<ID3DX11EffectTechnique> mTechNoTexture;
	wrl::ComPtr<ID3DX11EffectTechnique> mTechReflect;
	wrl::ComPtr<ID3DX11EffectTechnique> mTechReflectNoMSAA;
	wrl::ComPtr<ID3DX11EffectTechnique> mTechShadow;

	/*************��������************************/
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxWorldViewProj;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxViewProj;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxReflectWorldViewProj;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxWorld;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxWorldInvTranspose;
	wrl::ComPtr<ID3DX11EffectVectorVariable> mfxEyePosW;


	// ʵʱ��Ⱦʱ��
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxTime;

	// ����
	wrl::ComPtr<ID3DX11EffectVariable> mfxMaterial;

	wrl::ComPtr<ID3DX11EffectVariable>		mfxDirLights;
	wrl::ComPtr<ID3DX11EffectVariable>		mfxPointLights;
	wrl::ComPtr<ID3DX11EffectVariable>		mfxSpotLights;
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxNum_DL;
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxNum_PL;
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxNum_SL;

	// ����
	wrl::ComPtr<ID3DX11EffectShaderResourceVariable> mfxDiffuseMap;
	wrl::ComPtr<ID3DX11EffectShaderResourceVariable> mfxReflectMap;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxTexTransform;

	// ��Ӱ
	wrl::ComPtr<ID3DX11EffectVectorVariable> mfxShadowL;;
	/*************����������************************/

	// ͼ�����
	std::vector<Vertex> mVertices;
	std::vector<UINT> mIndexes;
	std::vector<UINT> mIndexCounts;
	std::vector<UINT> mVertexCounts;
	std::vector<bool> mHasTex;


	// ����
	std::vector<ReflectivePlane*> mirrors;

	// �ƹ������
	bool lightIsChange;
	std::vector<DirectionalLight>	mDirLights;
	std::vector<PointLight>			mPointLights;
	std::vector<SpotLight>			mSpotLights;
	UINT		num_DL;
	UINT		num_PL;
	UINT		num_SL;

};
#endif // !BASICEFFECT

