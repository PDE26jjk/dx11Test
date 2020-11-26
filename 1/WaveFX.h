#pragma once
#include "BasicEffect.h"
#include "Shape.h"
class WaveFX : public PDEeffect {
public:
	WaveFX(BasicEffect* be);
	~WaveFX();
	// Í¨¹ý PDEeffect ¼Ì³Ð
	virtual void Draw() override;
	virtual void LoadFx() override;
	virtual void SetTopoLayout() override;
	void Init(float Widht, float Height,float dx, float dt, float speed, float damping);

private:

	BasicEffect* mBe;
	PDEshape* mWave = nullptr;
	wrl::ComPtr<ID3DX11EffectTechnique> mTechWaveTex;
	wrl::ComPtr<ID3DX11EffectScalarVariable> mfxWaveConstants;
	wrl::ComPtr<ID3DX11EffectShaderResourceVariable> mfxPrevSolInput;
	wrl::ComPtr<ID3DX11EffectShaderResourceVariable> mfxCurrSolInput;
	wrl::ComPtr<ID3DX11EffectUnorderedAccessViewVariable> mfxNextSolOutput;


	std::vector<wrl::ComPtr<ID3D11ShaderResourceView>> mSRVs;
	std::vector<wrl::ComPtr<ID3D11UnorderedAccessView>> mUAVs;

	UINT mTexW;
	UINT mTexH;

	float mK1;
	float mK2;
	float mK3;
};