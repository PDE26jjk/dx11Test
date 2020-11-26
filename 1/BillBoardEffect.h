#pragma once
#include "effect.h"
#include "BasicEffect.h"
class Billboard :public PDEeffect {
public:
	Billboard(BasicEffect* be);
	// Í¨¹ý PDEeffect ¼Ì³Ð
	virtual void Draw() override;
	virtual void LoadFx() override;
	virtual void SetTopoLayout() override;
private:
	struct BillboardVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Size;

	};
	wrl::ComPtr<ID3DX11EffectTechnique> mTechBillboard;
	wrl::ComPtr<ID3DX11EffectShaderResourceVariable> mfxBillBoardMap;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxWorldViewProj;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxWorldInvTranspose;
};