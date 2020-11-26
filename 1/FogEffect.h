#pragma once
#include "effect.h"
#include "BasicEffect.h"
class FogEffect :public PDEeffect {
public:
	FogEffect(BasicEffect* be);
	// 通过 PDEeffect 继承
	virtual void Draw() override;
	virtual void LoadFx() override;
	virtual void SetTopoLayout() override;

	void SetFog(float fogStart, float fogRange, XMFLOAT4 fogColor);
private:

	//基础效果
	BasicEffect* mBasicEffect;

	//雾
	float		mFogStart;
	float		mFogRange;
	XMFLOAT4	mFogColor;
	bool		mFogIsChange;

	// 雾效果
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxFogStart;
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxFogRange;
	wrl::ComPtr<ID3DX11EffectVectorVariable> mfxFogColor;
	// 技术
	wrl::ComPtr<ID3DX11EffectTechnique> mTechNoFog;
};