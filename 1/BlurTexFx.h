#pragma once
#include "BasicEffect.h"
class BlurTexFx :
    public PDEeffect
{
public:
    BlurTexFx(Graphics* mGfx);
    void SetBlurCount(const UINT count) { this->blurCount = count; }
    void BlurInPlace(ID3D11DeviceContext* dc,
        ID3D11ShaderResourceView* inputSRV,
        ID3D11UnorderedAccessView* inputUAV,
        int blurCount);
    void SetInputMap(ID3D11ShaderResourceView* inputMap);
    void SetOutputMap(ID3D11UnorderedAccessView* OutputMap);

private:
    // 通过 PDEeffect 继承
    virtual void Draw() override;
    virtual void LoadFx() override;
    virtual void SetTopoLayout() override;

    void CreateCacheView();

    // 技术
    wrl::ComPtr<ID3DX11EffectTechnique> mHorzBlurTech;
    wrl::ComPtr<ID3DX11EffectTechnique> mVertBlurTech;

    // 纹理
    wrl::ComPtr<ID3DX11EffectShaderResourceVariable> mfxInput;
    wrl::ComPtr<ID3DX11EffectUnorderedAccessViewVariable> mfxOutput;

    // 
    UINT blurCount;
    UINT mWidth;
    UINT mHeight;
    DXGI_FORMAT mFormat;
    wrl::ComPtr<ID3D11ShaderResourceView> mBlurredOutputTexSRV;
    wrl::ComPtr<ID3D11UnorderedAccessView> mBlurredOutputTexUAV;
    
};

