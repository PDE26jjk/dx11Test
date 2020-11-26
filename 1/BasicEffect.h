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

	// 顶点缓冲
	wrl::ComPtr<ID3D11Buffer> mVB;
	// 索引缓冲
	wrl::ComPtr<ID3D11Buffer> mIB;
	UINT mVBstride; // 顶点缓冲步长
	UINT mVBoffset; // 顶点缓冲偏移

	// 两种栅格化方式：体渲染和线框渲染和开启背面
	static wrl::ComPtr<ID3D11RasterizerState> mWIREFRAME;
	static wrl::ComPtr<ID3D11RasterizerState> mSOLID;
	static wrl::ComPtr<ID3D11RasterizerState> mBACK;

	// 混合设定
	static wrl::ComPtr<ID3D11BlendState> mBlendStateSRC;
	static wrl::ComPtr<ID3D11BlendState> mBlendStateAdd;
	static wrl::ComPtr<ID3D11BlendState> mBlendStateSubtract;
	static wrl::ComPtr<ID3D11BlendState> mBlendStateMultiply;
	static wrl::ComPtr<ID3D11BlendState> mBlendStateTransparency;

	// 深度模板设定
	static wrl::ComPtr<ID3D11DepthStencilState> mNoDoubleBlendDSS;

	static bool mIsLoadState;

};

// 单例模式
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
	// 通过 PDEeffect 继承
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
	// 单例指针
	static BasicEffect* this_ptr;
	//基础图形
	std::vector<PDEshape*> pShapes;
	bool mIsLoadShapes;

	/***************技术**********************/
	wrl::ComPtr<ID3DX11EffectTechnique> mTechTexture;
	wrl::ComPtr<ID3DX11EffectTechnique> mTechNoTexture;
	wrl::ComPtr<ID3DX11EffectTechnique> mTechReflect;
	wrl::ComPtr<ID3DX11EffectTechnique> mTechReflectNoMSAA;
	wrl::ComPtr<ID3DX11EffectTechnique> mTechShadow;

	/*************常量缓冲************************/
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxWorldViewProj;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxViewProj;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxReflectWorldViewProj;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxWorld;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxWorldInvTranspose;
	wrl::ComPtr<ID3DX11EffectVectorVariable> mfxEyePosW;


	// 实时渲染时间
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxTime;

	// 材质
	wrl::ComPtr<ID3DX11EffectVariable> mfxMaterial;

	wrl::ComPtr<ID3DX11EffectVariable>		mfxDirLights;
	wrl::ComPtr<ID3DX11EffectVariable>		mfxPointLights;
	wrl::ComPtr<ID3DX11EffectVariable>		mfxSpotLights;
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxNum_DL;
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxNum_PL;
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxNum_SL;

	// 纹理
	wrl::ComPtr<ID3DX11EffectShaderResourceVariable> mfxDiffuseMap;
	wrl::ComPtr<ID3DX11EffectShaderResourceVariable> mfxReflectMap;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxTexTransform;

	// 阴影
	wrl::ComPtr<ID3DX11EffectVectorVariable> mfxShadowL;;
	/*************常量缓冲完************************/

	// 图形相关
	std::vector<Vertex> mVertices;
	std::vector<UINT> mIndexes;
	std::vector<UINT> mIndexCounts;
	std::vector<UINT> mVertexCounts;
	std::vector<bool> mHasTex;


	// 镜子
	std::vector<ReflectivePlane*> mirrors;

	// 灯光与材质
	bool lightIsChange;
	std::vector<DirectionalLight>	mDirLights;
	std::vector<PointLight>			mPointLights;
	std::vector<SpotLight>			mSpotLights;
	UINT		num_DL;
	UINT		num_PL;
	UINT		num_SL;

};
#endif // !BASICEFFECT

