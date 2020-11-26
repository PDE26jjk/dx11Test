#include "ReflectivePlane.h"
#include "GeometryGenerator.h"
using namespace DirectX;

ReflectivePlane::ReflectivePlane(XMFLOAT3 Position, XMFLOAT3 Direction, float width, float depth)
	:
	cameraReflect(),
	Position(Position)
{
	HasShadow = false;
	SetBlendType(BlendType::Transparency);
	MeshData meshData;
	GeometryGenerator().CreateGrid(-width, depth,100u,100u,meshData);

	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	this->meshData = meshData;
	this->init();
	XMVECTOR dir = XMLoadFloat3(&Direction);
	XMStoreFloat3(&this->Direction,XMVector3Normalize(dir));
	XMVECTOR pos = XMLoadFloat3(&Position);
	// 设置到指定位置
	XMVECTOR cross = XMVector3Cross(dir,up );
	Rotate(cross, XMVectorGetX(XMVector3AngleBetweenVectors(dir, up)));
	Move(Position.x, Position.y, Position.z);
	//cameraRefect.SetCamera(pos, pos + dir);
}

void ReflectivePlane::SetReflectView(ID3D11RenderTargetView* RTview, ID3D11ShaderResourceView* SRview) {
	this->RenderTargetView = wrl::ComPtr<ID3D11RenderTargetView>(RTview);
	this->ShaderResourceView = wrl::ComPtr<ID3D11ShaderResourceView>(SRview);
}

void ReflectivePlane::SetCamera(const XMVECTOR& pos, const XMVECTOR& target)
{
	cameraReflect.SetCamera(pos, target);
	XMMATRIX w = XMLoadFloat4x4(&W);
	refWVP = w * XMLoadFloat4x4(&cameraReflect.ViewProj);
	for (auto &v : meshData.Vertices) {
		//XMVECTOR pos = XMVectorSet(v.Position.x, v.Position.y, v.Position.z, 1.0f);

		//XMVECTOR posW = XMVector4Transform(pos,w);

		XMVECTOR sss = XMVector4Transform( XMLoadFloat3(&v.Position) , refWVP);
		sss/= XMVectorGetW(sss);
		int i = 1;
		++i;
		v.TexC.x = sss.m128_f32[0];
		v.TexC.y = sss.m128_f32[1];
	}

}

void ReflectivePlane::init()
{
	this->PDEshape::init();

}
