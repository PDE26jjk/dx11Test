#include "WaveFX.h"
#include "GeometryGenerator.h"
#include "DirectXTex.h"
WaveFX::WaveFX(BasicEffect* be)
	:
	PDEeffect("Wave.cso",be->GetGfx()),
	mBe(be)
{
	LoadFx();
	SetTopoLayout();
}

WaveFX::~WaveFX()
{
	if (mWave != nullptr) {
		delete mWave;
		mWave = nullptr;
	}
}
#include "ScreenGrab11.h"
void WaveFX::Draw()
{
	mTechWaveTex->GetDesc(&techDesc);

	float now = mGfx->GetRanderTime() * 1000.0f;
	mFX->GetVariableByName("time")->AsScalar()->SetFloat(now);


	// 按照 012 120 201 的顺序绑定3张纹理，以达成循环利用
	// P C N
	// 0 1 2
	// 1 2 0
	// 2 0 1
	static UINT texCount = 0;
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		HR(mfxPrevSolInput->SetResource(mSRVs[texCount].Get()));
		HR(mfxCurrSolInput->SetResource(mSRVs[(texCount+1u)%3].Get()));
		HR(mfxNextSolOutput->SetUnorderedAccessView(mUAVs[(texCount + 2u) % 3].Get()));
		HR(mTechWaveTex->GetPassByIndex(p)->Apply(0, mGfx->GetDeviceContext()));
		
		UINT numGroupsX = (UINT)ceilf(mTexW / 16.0f);
		UINT numGroupsY = (UINT)ceilf(mTexH / 16.0f);
		mGfx->GetDeviceContext()->Dispatch(numGroupsX, numGroupsY, 1);
	}
	++texCount;
	if (texCount > 2) texCount = 0;

	// Unbind the input texture from the CS for good housekeeping.
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	mGfx->GetDeviceContext()->CSSetShaderResources(0, 1, nullSRV);
	// Unbind output from compute shader (we are going to use
	// this output as an input in the next pass), and a resource
	// cannot be both an output and input at the same time.
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	mGfx->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	ID3D11Resource* tex;
	mSRVs[0]->GetResource(&tex);
	SaveDDSTextureToFile(mGfx->GetDeviceContext(), tex, L"wa1.dds");
	mSRVs[1]->GetResource(&tex);
	SaveDDSTextureToFile(mGfx->GetDeviceContext(), tex, L"wa2.dds");
	mSRVs[2]->GetResource(&tex);
	SaveDDSTextureToFile(mGfx->GetDeviceContext(), tex, L"wa3.dds");

	;
}

void WaveFX::LoadFx()
{
	mTech		= mFX->GetTechniqueByName("DrawWave");
	mTechWaveTex	= mFX->GetTechniqueByName("WaveTex");

	mfxWaveConstants = mFX->GetVariableByName("gWaveConstants")->AsScalar();
	mfxPrevSolInput = mFX->GetVariableByName("gPrevSolInput")->AsShaderResource();
	mfxCurrSolInput = mFX->GetVariableByName("gCurrSolInput")->AsShaderResource();
	mfxNextSolOutput = mFX->GetVariableByName("gNextSolOutput")->AsUnorderedAccessView();
	
	mTexW = 512;
	mTexH = 512;
	//DXGI_FORMAT format = DXGI_FORMAT_A8_UNORM;
	DXGI_FORMAT format = DXGI_FORMAT_R16_FLOAT;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < 3; ++i) {
		ScratchImage limage;
		limage.Initialize2D(format, mTexW, mTexH, 1, 1);
		wrl::ComPtr<ID3D11Texture2D> WaveTexture;
		CreateTextureEx(mGfx->GetDevice(), limage.GetImages(),
			limage.GetImageCount(), limage.GetMetadata(),
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
			0, 0, false, (ID3D11Resource**)WaveTexture.GetAddressOf());

		wrl::ComPtr<ID3D11ShaderResourceView> SRV;
		HR(pDevice->CreateShaderResourceView(WaveTexture.Get(),
			&srvDesc, SRV.GetAddressOf()));
		wrl::ComPtr<ID3D11UnorderedAccessView> UAV;

		HR(pDevice->CreateUnorderedAccessView(WaveTexture.Get(),
			&uavDesc, UAV.GetAddressOf()));
		mSRVs.push_back(SRV);
		mUAVs.push_back(UAV);
	}
	
	
	//DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	

	//LoadFromDDSFile(CA2W("source.dds"), DDS_FLAGS_NONE, nullptr, image);

	float WaveConstants[3] = { mK1,mK2, mK3 };
	HR(mfxWaveConstants->SetFloatArray(WaveConstants, 0, 3));
}

void WaveFX::SetTopoLayout()
{
	/*mTopoLayout = TopoLayout(TopoLayoutType::TrianglePNT);*/
}

void WaveFX::Init(float Widht, float Height, float dx, float dt, float speed, float damping)
{
	GeometryGenerator geo;
	float dG = 5.0f;
	UINT m = (UINT)ceilf(Widht / dG);
	UINT n = (UINT)ceilf(Height / dG);
	MeshData md;
	geo.CreateGrid(Widht, Height, m, n, md);
	 
	//mTimeStep = dt;
	//mSpatialStep = dx;

	float d = damping * dt + 2.0f;
	float e = (speed * speed) * (dt * dt) / (dx * dx);
	mK1 = (damping * dt - 2.0f) / d;
	mK2 = (4.0f - 8.0f * e) / d;
	mK3 = (2.0f * e) / d;

	mWave = new PDEshape(md);
	mWave->SetShadow(false);
	mWave->SetBlendType(BlendType::None);
	mBe->AddShape(mWave);
}
