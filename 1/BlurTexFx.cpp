#include "BlurTexFx.h"
#include "ScreenGrab11.h"

BlurTexFx::BlurTexFx(Graphics* gfx)
	:PDEeffect("BlurTex.cso",gfx),
	blurCount(1)
{
	LoadFx();
}

// 龙书p401
void BlurTexFx::BlurInPlace(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* inputSRV, ID3D11UnorderedAccessView* inputUAV, int blurCount) {
    {
        //
        // Run the compute shader to blur the offscreen texture.
        //
        for (int i = 0; i < blurCount; ++i)
        {
            // HORIZONTAL blur pass.
            mHorzBlurTech->GetDesc(&techDesc);
            for (UINT p = 0; p < techDesc.Passes; ++p)
            {
                SetInputMap(inputSRV);
                SetOutputMap(mBlurredOutputTexUAV.Get());
                mHorzBlurTech->GetPassByIndex(p)->Apply(0, dc);
                // How many groups do we need to dispatch to cover a
                // row of pixels, where each group covers 256 pixels
                // (the 256 is defined in the ComputeShader).
                UINT numGroupsX = (UINT)ceilf(mWidth / 256.0f);
                dc->Dispatch(numGroupsX, mHeight, 1);
            }

            // Unbind the input texture from the CS for good housekeeping.
            ID3D11ShaderResourceView* nullSRV[1] = { 0 };
            dc->CSSetShaderResources(0, 1, nullSRV);
            // Unbind output from compute shader (we are going to use
            // this output as an input in the next pass), and a resource
            // cannot be both an output and input at the same time.
            ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
            dc->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
            // VERTICAL blur pass.
            mVertBlurTech->GetDesc(&techDesc);
            for (UINT p = 0; p < techDesc.Passes; ++p)
            {
                SetInputMap(mBlurredOutputTexSRV.Get());
                SetOutputMap(inputUAV);
                mVertBlurTech->GetPassByIndex(p)->Apply(0, dc);
                // How many groups do we need to dispatch to cover a
                // column of pixels, where each group covers 256 pixels
                // (the 256 is defined in the ComputeShader).
                UINT numGroupsY = (UINT)ceilf(mHeight / 256.0f);
                dc->Dispatch(mWidth, numGroupsY, 1);
            }
            dc->CSSetShaderResources(0, 1, nullSRV);
            dc->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
        } // Disable compute shader.
            dc->CSSetShader(0, 0, 0);
    }
}

void BlurTexFx::SetInputMap(ID3D11ShaderResourceView* inputMap)
{
    mfxInput->SetResource(inputMap);
    ID3D11Texture2D* res;
    D3D11_TEXTURE2D_DESC Desc;
    inputMap->GetResource((ID3D11Resource**)&res);
    res->GetDesc(&Desc);

    if (mHeight != Desc.Height || mWidth != Desc.Width || mFormat != Desc.Format) {
        mHeight = Desc.Height;
        mWidth = Desc.Width;
        mFormat = Desc.Format;
        //更新尺寸，更新资源视图
        CreateCacheView();
    }
}

void BlurTexFx::SetOutputMap(ID3D11UnorderedAccessView* OutputMap)
{
    mfxOutput->SetUnorderedAccessView(OutputMap);
}

void BlurTexFx::LoadFx()
{
	mfxInput = mFX->GetVariableByName("gInput")->AsShaderResource();
	mfxOutput = mFX->GetVariableByName("gOutput")->AsUnorderedAccessView();
	mHorzBlurTech = mFX->GetTechniqueByName("HorzBlur");
	mVertBlurTech = mFX->GetTechniqueByName("VertBlur");

}
void BlurTexFx::Draw()
{

}



void BlurTexFx::SetTopoLayout()
{
}

void BlurTexFx::CreateCacheView()
{
    D3D11_TEXTURE2D_DESC blurredTexDesc;
    blurredTexDesc.Width = mWidth;
    blurredTexDesc.Height = mHeight;
    blurredTexDesc.MipLevels = 1;
    blurredTexDesc.ArraySize = 1;
    blurredTexDesc.Format = mFormat;
    blurredTexDesc.SampleDesc.Count = 1;
    blurredTexDesc.SampleDesc.Quality = 0;
    blurredTexDesc.Usage = D3D11_USAGE_DEFAULT;
    blurredTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    blurredTexDesc.CPUAccessFlags = 0;
    blurredTexDesc.MiscFlags = 0;

    ID3D11Texture2D* blurredTex = 0;
    HR(pDevice->CreateTexture2D(&blurredTexDesc, 0, &blurredTex));


    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = mFormat;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    HR(pDevice->CreateShaderResourceView(blurredTex,
        &srvDesc, mBlurredOutputTexSRV.GetAddressOf()));

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Format = mFormat;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;

    HR(pDevice->CreateUnorderedAccessView(blurredTex,
        &uavDesc, mBlurredOutputTexUAV.GetAddressOf()));
    // Views save a reference to the texture so we can release our reference.
    blurredTex->Release();
}
