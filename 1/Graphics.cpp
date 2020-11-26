#include "Graphics.h"
#include "Shape.h"
#include "GeometryGenerator.h"
#include "DDSTextureLoader11.h"
#include "ScreenGrab11.h"
#include "TopoLayout.h"
#include "Effect.h"
Graphics::Graphics(HWND hWnd, UINT width,UINT height, bool enable4xMsaa)
	:
	mWinWidth(width),
	mWinHeight(height),
	mBGColor(0.0f, 1.0f, 0.0f, 1.0f), 
	gfxTimer(),
	mEnable4xMsaa(enable4xMsaa),

	cameraIsChange(true),

	mMainCamera(Camera())

	//mBasicEffect(this)
{

	//mEnable4xMsaa = false;
	//以下方法开不了抗锯齿
	//DXGI_SWAP_CHAIN_DESC sd;
	//sd.BufferDesc.Width = 0;
	//sd.BufferDesc.Height = 0;
	//sd.BufferDesc.RefreshRate.Numerator = 60;
	//sd.BufferDesc.RefreshRate.Denominator = 1;
	//sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//// 
	//sd.SampleDesc.Count = 1;
	//sd.SampleDesc.Quality = 0;

	//sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//sd.BufferCount = 1; // 双缓冲
	//sd.OutputWindow = hWnd;
	//sd.Windowed = true;
	//sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//sd.Flags = 0;

	//HR(D3D11CreateDeviceAndSwapChain(
	//	nullptr,
	//	D3D_DRIVER_TYPE_HARDWARE,
	//	nullptr,
	//	0,
	//	nullptr,
	//	0,
	//	D3D11_SDK_VERSION,
	//	&sd,
	//	&pSwap,
	//	&pDevice,
	//	nullptr,
	//	&pContext
	//));

// 初始化设备、上下文、交换链
#pragma region device,context,swapchain

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel;
	HR(D3D11CreateDevice(
		0,// default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		0,// no software device
		createDeviceFlags,
		0, 0,// default feature level array
		D3D11_SDK_VERSION,
		&pDevice,
		&featureLevel,
		&pContext));

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBoxA(0, "Direct3D Feature Level 11 unsupported.", 0, 0);
		return;
	}
	
	// HR：书作者提供来方便判断hr是否成功的宏
	HR(pDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = 0; 
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// Use 4X MSAA?
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		// m4xMsaaQuality is returned via CheckMultisampleQualityLevels().
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	//No MSAA
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	sd.Flags = 0;

	wrl::ComPtr<IDXGIDevice> dxgiDevice = 0;
	HR(pDevice->QueryInterface(__uuidof(IDXGIDevice),
		(void**)&dxgiDevice));

	wrl::ComPtr<IDXGIAdapter> dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter),(void**) & dxgiAdapter));

	// Finally got the IDXGIFactory interface.
	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory),(void**) & dxgiFactory));

	// Now, create the swap chain.
	HR(dxgiFactory->CreateSwapChain(pDevice.Get(), &sd, &pSwap));
#pragma endregion
	//mBasicEffect.init();
	//BasicEffect mBasicEffect(this);
	/****************************创建深度模板视图**************************/
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = mWinWidth;
	depthStencilDesc.Height = mWinHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}//No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;


	HR(pDevice->CreateTexture2D(&depthStencilDesc, // Description of texture to create.
		0,// 填充数据，由于是深度纹理，为0
		&mDepthStencilBuffer)); // Return pointer to depth/stencil buffer.
	HR(pDevice->CreateDepthStencilView(
		mDepthStencilBuffer.Get(), // Resource we want to create a view to.
		0,// D3D11_DEPTH_STENCIL_VIEW_DESC，0表示使用纹理的数据类型（不是typeless）
		mDepthStencilView.GetAddressOf())); // Return depth/stencil view

	/****************************创建深度模板视图完**************************/
	/****************************定义主渲染目标视图***********************/
	Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
	//ID3D11Resource* pBackBuffer = 
	pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
	pDevice->CreateRenderTargetView(
		pBackBuffer.Get(),
		nullptr,
		&pMainTarget
	);

	pContext->OMSetRenderTargets(1u, pMainTarget.GetAddressOf(), mDepthStencilView.Get());
	/*****************************定义主渲染目标视图完***********************/
	//pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(),nullptr);
	/****************************定义主视口**************************/
	
	mMainViewPort.TopLeftX = 0.0f;
	mMainViewPort.TopLeftY = 0.0f;
	mMainViewPort.Width = float(mWinWidth);
	mMainViewPort.Height = float(mWinHeight);
	mMainViewPort.MinDepth = 0.0f;
	mMainViewPort.MaxDepth = 1.0f;

	pContext->RSSetViewports(1, &mMainViewPort);

	/****************************定义主视口完**************************/
	

	
	//D3D11_RECT rects = { 100, 100, 400, 400 };
	//pContext->RSSetScissorRects(1, &rects);
}




Graphics::~Graphics()
{
	//if (mDirLights.size() > 0) {
	//	for (int i = 0; i < mDirLights.size(); ++i) {
	//		if (&mDirLights[i] != nullptr)
	//			delete &mDirLights[i];
	//	}
	//}
}



void Graphics::SetCamera(Camera camera) {
	this->mMainCamera = camera;
	cameraIsChange = true;
}


void Graphics::UpdateCamera(float mTheta, float mPhi, float mRadius)
{
	mMainCamera.SetCamera(mTheta, mPhi, mRadius);
	cameraIsChange = true;
}

void Graphics::EndFrame()
{
	if (cameraIsChange) {
		cameraIsChange = false;
	}
	pSwap->Present(1u, 0u);
}


void Graphics::DrawFrame()
{
	// 更新资源
	//mTech->GetDesc(&techDesc);
	ClearBuffer(pMainTarget.Get());
	for (auto e : effects) {
		e->Draw();
	}

}

void Graphics::ClearBuffer(ID3D11RenderTargetView* target) {
	XMFLOAT4 cc(0.0f, 0.0f, 1.0f, 0.5f);
	pContext->ClearRenderTargetView(target, reinterpret_cast<float*>(&mBGColor));
	pContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


#pragma region 测试代码



#pragma endregion 