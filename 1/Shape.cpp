#include "Shape.h"
#include <algorithm>
#include "BasicEffect.h"
PDEshape::PDEshape(MeshData& meshData)
	:
	meshData(meshData),
	blendType(BlendType::None),
	mEffect(BasicEffect::get_instance())
{
	init();
}

void PDEshape::init()
{
	XMStoreFloat4x4(&W, XMMatrixIdentity());
	XMStoreFloat4x4(&TexW, XMMatrixIdentity());
	initMat();
}

void PDEshape::Move(float x, float y, float z)
{
	XMStoreFloat4x4(&W,XMLoadFloat4x4(&W) * XMMatrixTranslation(x, y, z));
}

void PDEshape::Scale(float x, float y, float z)
{
	XMStoreFloat4x4(&W, XMLoadFloat4x4(&W) * XMMatrixScaling(x, y, z));
	
}
void PDEshape::Rotate(XMVECTOR A, float angle) {
	XMStoreFloat4x4(&W, XMLoadFloat4x4(&W) * XMMatrixRotationAxis(A, angle));
}

void PDEshape::MoveTex(float u, float v)
{
	XMStoreFloat4x4(&TexW, XMLoadFloat4x4(&TexW) * XMMatrixTranslation(u, v, 1.0f));
}

void PDEshape::ScaleTex(float x, float y)
{
	XMStoreFloat4x4(&TexW, XMLoadFloat4x4(&TexW) * XMMatrixScaling(x, y, 1.0f));

}
void PDEshape::RotateTex(XMVECTOR A, float angle) {
	XMStoreFloat4x4(&TexW, XMLoadFloat4x4(&TexW) * XMMatrixRotationAxis(A, angle));
}

void PDEshape::ResetTex()
{
	XMStoreFloat4x4(&TexW, XMMatrixIdentity());
}

void PDEshape::SetMatOpaque(float opaque) { opaque = std::clamp(opaque, 0.0f, 1.0f); this->Mat.Diffuse.w = opaque; }

XMMATRIX PDEshape::InverseTranspose(CXMMATRIX M) {
	XMMATRIX A = M;
	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	return XMMatrixTranspose(XMMatrixInverse(&det, A));
}



void PDEshape::initMat()
{
	Mat.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Mat.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 16.0f);
	Mat.Reflect = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}
