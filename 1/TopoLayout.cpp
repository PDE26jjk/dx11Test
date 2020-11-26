#include "TopoLayout.h"

TopoLayout::TopoLayout(TopoLayoutType type)
	:type(type)
{
	
}

ID3D11InputLayout* TopoLayout::GetLayout(ID3D11Device* pDevice,ID3DX11EffectTechnique* tech)
{
	D3D11_INPUT_ELEMENT_DESC* vertexDesc = nullptr;
	UINT numEle = 0u;// ¼¸¸öÓïÒå
	switch (type)
	{
	case TopoLayoutType::BillbordPS:{
			topo = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
			static D3D11_INPUT_ELEMENT_DESC vdBillbord[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			vertexDesc = vdBillbord;
			numEle = 2;
			break;
		}
	case TopoLayoutType::TrianglePNT: {
			topo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			static D3D11_INPUT_ELEMENT_DESC vdTriangle[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			numEle = 3;
			vertexDesc = vdTriangle;
			break;
		}
	case TopoLayoutType::HillTessP: {
		topo = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
		static D3D11_INPUT_ELEMENT_DESC vdHillTess[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		numEle = 1;
		vertexDesc = vdHillTess;
		break;
	}
	default:
		return nullptr;
	}
	// Create the input layout
	D3DX11_PASS_DESC passDesc;
	tech->GetPassByIndex(0)->GetDesc(&passDesc);

	HR(pDevice->CreateInputLayout(vertexDesc, numEle,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, mInputLayout.GetAddressOf()));
	return mInputLayout.Get();
}
