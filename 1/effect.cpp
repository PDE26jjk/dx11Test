
#include "BasicEffect.h"


PDEeffect::PDEeffect(const char *FXfilename, Graphics* gfx)
	:
	mGfx(gfx)

{
	pDevice = wrl::ComPtr<ID3D11Device>(mGfx->GetDevice());
	pContext = wrl::ComPtr<ID3D11DeviceContext>(mGfx->GetDeviceContext());
	wrl::ComPtr<ID3DBlob> compiledShader;
	HR(D3DReadFileToBlob(CA2W(FXfilename), &compiledShader));
	HR(D3DX11CreateEffectFromMemory(
		compiledShader->GetBufferPointer(),
		compiledShader->GetBufferSize(),
		0,
		pDevice.Get(), &mFX)); 
	CreateState();
}


void PDEeffect::Bind()
{

	// 绑定到管道
	pContext->IASetVertexBuffers(0, 1, mVB.GetAddressOf(),
		&mVBstride, &mVBoffset);
	if(mIB)
	pContext->IASetIndexBuffer(mIB.Get(),
		DXGI_FORMAT_R32_UINT, 0);
	if (mTopoLayout.GetType() != TopoLayoutType::UnDefine) {
		pContext->IASetInputLayout(mTopoLayout.GetLayout(pDevice.Get(), mTech.Get()));
		pContext->IASetPrimitiveTopology(mTopoLayout.GetTopo());
	}
}


wrl::ComPtr<ID3D11RasterizerState> PDEeffect::mWIREFRAME = nullptr;
wrl::ComPtr<ID3D11RasterizerState> PDEeffect::mSOLID = nullptr;
wrl::ComPtr<ID3D11RasterizerState> PDEeffect::mBACK = nullptr;

wrl::ComPtr<ID3D11BlendState> PDEeffect::mBlendStateSRC = nullptr;
wrl::ComPtr<ID3D11BlendState> PDEeffect::mBlendStateAdd = nullptr;
wrl::ComPtr<ID3D11BlendState> PDEeffect::mBlendStateSubtract = nullptr;
wrl::ComPtr<ID3D11BlendState> PDEeffect::mBlendStateMultiply = nullptr;
wrl::ComPtr<ID3D11BlendState> PDEeffect::mBlendStateTransparency = nullptr;

wrl::ComPtr<ID3D11DepthStencilState> PDEeffect::mNoDoubleBlendDSS = nullptr;

bool PDEeffect::mIsLoadState = false;

void PDEeffect::CreateState()
{
	if (!mIsLoadState) {
		
		CreateAllBlendState();
		CreateAllDepthStencilState();
		CreateAllRasterizerState();
		mIsLoadState = true;
	}
	
}


inline void PDEeffect::CreateAllRasterizerState()
{
	/****************************创建栅格化设定**************************/

	// 线框渲染
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = true;
	//rsDesc.ScissorEnable = true;
	HR(pDevice->CreateRasterizerState(&rsDesc, &mWIREFRAME));

	// 体渲染
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = true;
	//rsDesc.ScissorEnable = true;
	HR(pDevice->CreateRasterizerState(&rsDesc, &mSOLID));
	rsDesc.CullMode = D3D11_CULL_NONE;
	HR(pDevice->CreateRasterizerState(&rsDesc, &mBACK));
	/****************************创建栅格化设定完**************************/
}



inline void PDEeffect::CreateAllBlendState()
{
	D3D11_BLEND_DESC blendDesc = { 0 };
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL
		;

	HR(pDevice->CreateBlendState(&blendDesc, mBlendStateSRC.GetAddressOf()));

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	HR(pDevice->CreateBlendState(&blendDesc, mBlendStateAdd.GetAddressOf()));

	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
	HR(pDevice->CreateBlendState(&blendDesc, mBlendStateSubtract.GetAddressOf()));

	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
	HR(pDevice->CreateBlendState(&blendDesc, mBlendStateMultiply.GetAddressOf()));

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	HR(pDevice->CreateBlendState(&blendDesc, mBlendStateTransparency.GetAddressOf()));
}


inline void PDEeffect::CreateAllDepthStencilState()
{
	//p348
	D3D11_DEPTH_STENCIL_DESC noDoubleBlendDesc;
	noDoubleBlendDesc.DepthEnable = true;
	noDoubleBlendDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	noDoubleBlendDesc.DepthFunc = D3D11_COMPARISON_LESS;
	noDoubleBlendDesc.StencilEnable = true;
	noDoubleBlendDesc.StencilReadMask = 0xff;
	noDoubleBlendDesc.StencilWriteMask = 0xff;
	noDoubleBlendDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	// We are not rendering backfacing polygons, so these settings do not matter.
	noDoubleBlendDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	HR(pDevice->CreateDepthStencilState(&noDoubleBlendDesc, mNoDoubleBlendDSS.GetAddressOf()));
}

PDEeffect::PDEeffect()
	:mTopoLayout()
{
}


