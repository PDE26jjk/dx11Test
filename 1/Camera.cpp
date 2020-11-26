#include "Camera.h"
using namespace DirectX;

Camera::Camera()
{
	float mTheta = 1.5f * XM_PI;
	float mPhi = 0.25f * XM_PI;
	float mRadius = 5.0f;
	float AspectRatio = 1.3333333f;

	new(this) Camera(mTheta, mPhi, mRadius, AspectRatio);
}

Camera::Camera(float mTheta, float mPhi, float mRadius, float AspectRatio)
	:
	mTheta(mTheta),
	mPhi(mPhi),
	mRadius(mRadius)
{
	target = XMVectorZero();
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	SetAspectRatio(AspectRatio);
	SetCamera(mTheta, mPhi, mRadius);
}

void Camera::SetCamera(float mTheta, float mPhi, float mRadius)
{
	this->mTheta = mTheta;
	this->mPhi = mPhi;
	this->mRadius = mRadius;

	XMMATRIX PM = XMLoadFloat4x4(&P);
	
	// 计算局部位置
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);
	float y = mRadius * cosf(mPhi);
	XMVECTOR lpos = XMVectorSet(x, y, z, 1.0f);

	// 计算全局位置
	pos = lpos + target;

	XMFLOAT4 position;
	XMStoreFloat4(&position, pos);

	mEyePosW = XMFLOAT3(position.x, position.y, position.z);

	XMMATRIX VM = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&V, VM);

	XMMATRIX ViewProjM = VM * PM;
	XMStoreFloat4x4(&ViewProj, ViewProjM);

}

void Camera::SetCamera(const XMVECTOR& pos, const XMVECTOR& target)
{

	XMMATRIX PM = XMLoadFloat4x4(&P);

	XMFLOAT4 position;
	XMStoreFloat4(&position, pos);
	
	// 计算视矩阵
	float x = position.x;
	float z = position.z;
	float y = position.y;

	mEyePosW = XMFLOAT3(x, y, z);

	this->pos = XMVectorSet(x, y, z, 1.0f);
	this->target = target;

	XMMATRIX VM = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&V, VM);
	
	XMMATRIX ViewProjM = VM * PM;
	XMStoreFloat4x4(&ViewProj, ViewProjM);

	XMVECTOR lpos = pos - target;
	
	float mRadius = XMVectorGetX(XMVector4LengthEst(lpos));
	float mPhi = XMVectorGetX(XMVector4AngleBetweenVectors(lpos,up));
	float mTheta = XMVectorGetX(XMVector4AngleBetweenVectors(
		XMVectorSetY(lpos, 0.0f),
		XMVectorSet(1.0f,0.0f,0.0f,0.0f)));
	this->mTheta = mTheta;
	this->mPhi = mPhi;
	this->mRadius = mRadius;
}

void Camera::SetAspectRatio(const float& AspectRatio)
{
	this->AspectRatio = AspectRatio;
	// 计算投影矩阵
	XMMATRIX PM = XMMatrixPerspectiveFovLH(0.25f * XM_PI,
		AspectRatio, 0.01f, 100000.0f);
	XMStoreFloat4x4(&P, PM);
}
