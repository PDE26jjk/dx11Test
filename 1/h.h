#pragma once

#define NOMINMAX
#include <windows.h>
#include "atlbase.h"
#include "atlstr.h"
#include <directxmath.h>
#include "d3dx11effect.h"
#include <wrl/client.h>
#include <vector>
#include <d3dcompiler.h>


#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"Effects11d.lib")
#pragma comment(lib,"D3DCompiler.lib")
#define DEBUG

HRESULT WINAPI DXTraceW(_In_z_ const WCHAR* strFile, _In_ DWORD dwLine, _In_ HRESULT hr,
    _In_opt_ const WCHAR* strMsg, _In_ bool bPopMsgBox);




#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTraceW(__FILEW__, (DWORD)__LINE__, hr, L#x, true); \
		}                                                      \
	}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 


namespace Colors
{
    using namespace DirectX;
    XMGLOBALCONST XMVECTORF32 White = { 1.0f, 1.0f, 1.0f, 1.0f };
    XMGLOBALCONST XMVECTORF32 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
    XMGLOBALCONST XMVECTORF32 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
    XMGLOBALCONST XMVECTORF32 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
    XMGLOBALCONST XMVECTORF32 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
    XMGLOBALCONST XMVECTORF32 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
    XMGLOBALCONST XMVECTORF32 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
    XMGLOBALCONST XMVECTORF32 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

    XMGLOBALCONST XMVECTORF32 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
    XMGLOBALCONST XMVECTORF32 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
}
namespace Colors2
{
    using namespace DirectX;
    XMGLOBALCONST XMFLOAT4 White = { 1.0f, 1.0f, 1.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
    XMGLOBALCONST XMFLOAT4 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

    XMGLOBALCONST XMVECTORF32 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
    XMGLOBALCONST XMVECTORF32 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
}
namespace Coordinate
{
    using namespace DirectX;
    XMGLOBALCONST XMVECTORF32 X = { 1.0f, 0.0f, 0.0f, 0.0f };
    XMGLOBALCONST XMVECTORF32 Y = { 0.0f, 1.0f, 0.0f, 0.0f };
    XMGLOBALCONST XMVECTORF32 Z = { 0.0f, 0.0f, 1.0f, 0.0f };
}