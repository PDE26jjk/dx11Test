#include "BillBoardEffect.h"
#include "DDSTextureLoader11.h"
#include "ScreenGrab11.h"
Billboard::Billboard(BasicEffect* be) { 
	this->mFX = be->GetFX(); 
	this->mGfx = be->GetGfx(); 
	this->pContext = mGfx->GetDeviceContext();
	this->pDevice = mGfx->GetDevice();
	SetTopoLayout();
	LoadFx();
}
void Billboard::Draw()
{
	Bind();
	mTech = mTechBillboard;
	mTech->GetDesc(&techDesc);
	UINT allVertex = 4;
	//InitBillboard(allVertex);


	pContext->OMSetRenderTargets(1u, mGfx->GetpMainTarget(), mGfx->GetDepthStencilView());
	// 渲染方式
	pContext->RSSetState(mSOLID.Get());
	static float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	pContext->OMSetBlendState(mBlendStateTransparency.Get(), blendFactors, 0xffffffff);

	/****************************更新坐标并绘制**************************/
	XMMATRIX ViewProjM = XMLoadFloat4x4(&mGfx->GetMainCamera().ViewProj);
	XMMATRIX world = XMMatrixIdentity();
	XMMATRIX worldViewProj = world * ViewProjM;

	mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&PDEshape::InverseTranspose(world)));
	mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

	

	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex(p)->Apply(0, pContext.Get());

		pContext->Draw(allVertex,0u);
	}
	/****************************更新坐标并绘制完**************************/
	pContext->RSSetState(0);
	
}

void Billboard::LoadFx()
{
	mTech = mTechBillboard = mFX->GetTechniqueByName("Billboard");
	mfxBillBoardMap = mFX->GetVariableByName("gBillboardMap")->AsShaderResource();
	mfxWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	UINT allVertex = 4;
	float w = 100.0f;
	float h = 100.0f;
	BillboardVertex vs[] =
	{
		{ { 2000.0f,0.0f,0.0f },{ w,h } },
		{ { 0.0f,0.0f,0.0f },{ w,h } },
		{ { -500.0f,0.0f,500.0f },{ w,h } },
		{ { 1000.0f,0.0f,1000.0f },{ w,h } },
	};


	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(BillboardVertex) * allVertex;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vs[0];

	HR(pDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	mVBstride = sizeof(BillboardVertex);
	mVBoffset = 0u;

	ID3D11Resource* res;
	ID3D11ShaderResourceView* pSRV;
	//DDS_ALPHA_MODE alpha = DDS_ALPHA_MODE::DDS_ALPHA_MODE_STRAIGHT;
	HR(CreateDDSTextureFromFile(pDevice.Get(), L"misuhara.dds", &res, &pSRV, 0u));

	HR(mfxBillBoardMap->SetResource(pSRV));
	res->Release();
	pSRV->Release();

}

void Billboard::SetTopoLayout()
{
	mTopoLayout = TopoLayout(TopoLayoutType::BillbordPS);
}
