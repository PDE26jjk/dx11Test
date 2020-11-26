#include "FogEffect.h"

FogEffect::FogEffect(BasicEffect* be)
	:
	mFogIsChange(true),

	mFogStart(0.0f),
	mFogRange(1000.0f),
	mFogColor(0.0f, 0.0f, 0.0f, 1.0f)

{
	mBasicEffect = be;
	this->mFX = be->GetFX();
	LoadFx();
}

void FogEffect::Draw()
{
	if (mFogIsChange) {
		mfxFogStart->SetFloat(mFogStart);
		mfxFogRange->SetFloat(mFogRange);
		mfxFogColor->SetRawValue(&mFogColor, 0, sizeof(mFogColor));
		mFogIsChange = false;
	}
}

void FogEffect::LoadFx()
{
	mTechNoFog = mFX->GetTechniqueByName("NOFog");

	mfxFogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	mfxFogRange = mFX->GetVariableByName("gFogRange")->AsScalar();
	mfxFogColor = mFX->GetVariableByName("gFogColor")->AsVector();
}

void FogEffect::SetTopoLayout()
{
}

void FogEffect::SetFog(float fogStart, float fogRange, XMFLOAT4 fogColor)
{
	mFogIsChange = true;
	mFogStart = fogStart;
	mFogRange = fogRange;
	mFogColor = fogColor;
}
