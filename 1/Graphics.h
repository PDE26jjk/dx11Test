#ifndef GRAPHICS
#define GRAPHICS
#include "h.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <memory>
#include "timer.h"
#include "light.h"
#include "Camera.h"
#include "ReflectivePlane.h"


#include "BasicEffect.h"

class PDEeffect;
//class BasicEffect {}
namespace wrl = Microsoft::WRL;
class Graphics {

public:
	Graphics(HWND hWnd, UINT width, UINT height, bool enable4xMsaa);

	void LoadFx();

	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void AddEffect(PDEeffect* ef) { effects.push_back(ef); }


	void SetAspectRatio(float AspectRatio) { this->AspectRatio = AspectRatio; mMainCamera.SetAspectRatio(AspectRatio); }
	void SetBackgroundColor(XMFLOAT4 color) { this->mBGColor = color; }
	void SetCamera(Camera camera);
	void UpdateCamera(float mTheta, float mPhi, float mRadius);
	void EndFrame();
	//void DrawBox();
	//void DrawShapes(ID3D11RenderTargetView* target, Camera& camera);
	//void DrawMirrors(ID3D11RenderTargetView* target, Camera& camera);
	void DrawFrame();

	void ClearBuffer(ID3D11RenderTargetView* target);

	//std::vector<PDEshape*>& Shapes() { return pShapes; }

public:
	bool IsUse4xMsaa() { return mEnable4xMsaa; }
	UINT Get4xMsaaQuality() { return m4xMsaaQuality; }
	UINT GetWinWidth() { return mWinWidth; }
	UINT GetWinHeight() { return mWinHeight; }
	ID3D11Device* GetDevice() { return pDevice.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return pContext.Get(); }
	Camera& GetMainCamera() { return mMainCamera; }

	ID3D11DepthStencilView* GetDepthStencilView() { return mDepthStencilView.Get(); }
	D3D11_VIEWPORT& GetMainViewPort() { return mMainViewPort; }
	ID3D11RenderTargetView** GetpMainTarget() { return pMainTarget.GetAddressOf(); };
	float GetRanderTime() {return gfxTimer.Peek();}


private:
	//std::vector<UINT>& SortShapes(Camera& camera) const;
private:
	UINT mWinWidth;
	UINT mWinHeight;
	XMFLOAT4 mBGColor;
	bool mEnable4xMsaa;
	UINT m4xMsaaQuality;
	// 主视口
	D3D11_VIEWPORT mMainViewPort;

	wrl::ComPtr<ID3D11Device> pDevice = nullptr;
	wrl::ComPtr<IDXGISwapChain> pSwap = nullptr;
	wrl::ComPtr<ID3D11DeviceContext> pContext = nullptr;
	wrl::ComPtr<ID3D11RenderTargetView> pMainTarget = nullptr;

	float AspectRatio = 1.333333f;

	// 摄像机
	Camera mMainCamera;
	bool cameraIsChange;


	std::vector<PDEeffect*> effects;

	wrl::ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
	wrl::ComPtr<ID3D11DepthStencilView> mDepthStencilView;
	

	// 计时器
	Timer gfxTimer;

	

#pragma region 测试代码
public:
	void InitBillboard(const UINT& allVertex);
	void drawBillboard();

#pragma endregion
};
#endif // !GRAPHICS

