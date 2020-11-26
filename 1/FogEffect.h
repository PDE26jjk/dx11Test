#pragma once
#include "effect.h"
#include "BasicEffect.h"
class FogEffect :public PDEeffect {
public:
	FogEffect(BasicEffect* be);
	// ͨ�� PDEeffect �̳�
	virtual void Draw() override;
	virtual void LoadFx() override;
	virtual void SetTopoLayout() override;

	void SetFog(float fogStart, float fogRange, XMFLOAT4 fogColor);
private:

	//����Ч��
	BasicEffect* mBasicEffect;

	//��
	float		mFogStart;
	float		mFogRange;
	XMFLOAT4	mFogColor;
	bool		mFogIsChange;

	// ��Ч��
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxFogStart;
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxFogRange;
	wrl::ComPtr<ID3DX11EffectVectorVariable> mfxFogColor;
	// ����
	wrl::ComPtr<ID3DX11EffectTechnique> mTechNoFog;
};