#pragma once
#include "h.h"
#include <wrl/client.h>
#include <d3d11.h>
#include "d3dx11effect.h"
namespace wrl = Microsoft::WRL;
using namespace DirectX;
enum TopoLayoutType {
	TrianglePNT,// 三角形 位置法线贴图
	BillbordPS,// 告示板 位置(地面中心) 长宽
	HillTessP,
	UnDefine
};
class TopoLayout {
public:
	TopoLayout():type(TopoLayoutType::UnDefine) {};
	TopoLayout(TopoLayoutType type);
	ID3D11InputLayout* GetLayout(ID3D11Device* pDevice, ID3DX11EffectTechnique* tech);
	D3D_PRIMITIVE_TOPOLOGY& GetTopo() { return topo; };
	TopoLayoutType GetType() { return type; }
private:
	wrl::ComPtr<ID3D11InputLayout> mInputLayout;
	TopoLayoutType type;

	D3D_PRIMITIVE_TOPOLOGY topo;
};