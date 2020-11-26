#include "hillTess.h"

HillTess::HillTess(Graphics* gpx)
	:
	PDEeffect("HillTess.cso",gpx)
{
	LoadFx();
	SetTopoLayout();
}

void HillTess::Draw()
{
	Bind();
	mTech->GetDesc(&techDesc);

	pContext->OMSetRenderTargets(1u, mGfx->GetpMainTarget(), mGfx->GetDepthStencilView());
	// 渲染方式
	pContext->RSSetState(mWIREFRAME.Get());
	static float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	pContext->OMSetBlendState(mBlendStateTransparency.Get(), blendFactors, 0xffffffff);

	/****************************更新坐标并绘制**************************/
	XMMATRIX ViewProjM = XMLoadFloat4x4(&mGfx->GetMainCamera().ViewProj);
	XMMATRIX world = XMMatrixIdentity();
	XMMATRIX worldViewProj = world * ViewProjM;
	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
	mfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
	mfxEyePosW->SetRawValue(&mGfx->GetMainCamera().mEyePosW, 0, sizeof(mGfx->GetMainCamera().mEyePosW));

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, pContext.Get());
		pContext->Draw(4, 0u);
	}
	/****************************更新坐标并绘制完**************************/
	pContext->RSSetState(0);
}

void HillTess::LoadFx()
{
	mTech= mFX->GetTechniqueByName("Tess");
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(XMFLOAT3) * 4;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	XMFLOAT3 vertices[4] =
	{
		XMFLOAT3(-10.0f, 0.0f, +10.0f),
		XMFLOAT3(+10.0f, 0.0f, +10.0f),
		XMFLOAT3(+10.0f, 0.0f, -10.0f),
		XMFLOAT3(-10.0f, 0.0f, -10.0f),
	};
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;
	HR(pDevice->CreateBuffer(&vbd, &vinitData, &mVB));
	mVBstride = sizeof(XMFLOAT3);
	mVBoffset = 0;
}

void HillTess::SetTopoLayout()
{
	mTopoLayout = TopoLayout(TopoLayoutType::HillTessP);
}
