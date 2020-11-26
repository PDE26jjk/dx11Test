#pragma once
#include "BasicEffect.h"

class HillTess :public PDEeffect {
public:
	// Í¨¹ý PDEeffect ¼Ì³Ð
	HillTess(Graphics* mGpx);
private:
	virtual void Draw() override;
	virtual void LoadFx() override;
	virtual void SetTopoLayout() override;

private:
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxWorldViewProj;
	wrl::ComPtr<ID3DX11EffectMatrixVariable> mfxWorld;
	wrl::ComPtr<ID3DX11EffectVectorVariable> mfxEyePosW;
};