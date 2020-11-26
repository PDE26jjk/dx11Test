#include "Camera.h"
using namespace DirectX;

Camera::Camera()
{
	XMVECTOR pos = XMVectorZero();
	XMVECTOR dir = XMVectorSet(0.0f,0.0f,1.0f,0.0f);
	float AspectRatio = 1.33333333f;

	new(this) Camera(pos, dir, AspectRatio);
}


Camera::Camera(const XMVECTOR& pos, const XMVECTOR& dir, float AspectRatio)
	:
	FovAngleY(0.25f * XM_PI),
	NearZ(0.01f), 
	FarZ(10000.0f),
	AspectRatio(AspectRatio)
{
	XMStoreFloat4(&up,Coordinate::Y);
	XMStoreFloat4(&this->pos, pos);
	XMStoreFloat4(&this->dir, dir);
	SetAspectRatio(AspectRatio);
	SetCamera(pos, dir);
}


void Camera::SetCamera(const XMVECTOR& pos, const XMVECTOR& dir)
{


	XMFLOAT4 position;
	XMStoreFloat4(&position, pos);
	
	// 计算视矩阵
	float x = position.x;
	float z = position.z;
	float y = position.y;

	this->pos = XMFLOAT4(x, y, z,1.0f);
	XMStoreFloat4(&this->dir, dir);

	UpdateViewMatrix();
	
}

void Camera::MoveCamera(const XMVECTOR& offset)
{
	XMVECTOR pos = XMLoadFloat4(&this->pos);
	pos += offset;
	pos.m128_f32[3] = 1.0f;
	XMStoreFloat4(&this->pos, pos);
	UpdateViewMatrix();
}

void Camera::RollCamera(const XMVECTOR& A, float angle)
{
	XMVECTOR dir = XMLoadFloat4(&this->dir);
	dir = XMVector3Transform(dir, XMMatrixRotationAxis(A, angle));
	float dirY = XMVectorGetY(dir);
	if (dirY > 0.99f || dirY < -0.99f) return;
	XMStoreFloat4(&this->dir, dir);
	UpdateViewMatrix();
}

void Camera::SetAspectRatio(const float& AspectRatio)
{
	this->AspectRatio = AspectRatio;
	// 计算投影矩阵
	XMMATRIX PM = XMMatrixPerspectiveFovLH(FovAngleY,
		AspectRatio, NearZ, FarZ);
	XMStoreFloat4x4(&P, PM);
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR pos = XMLoadFloat4(&this->pos);
	XMVECTOR dir = XMLoadFloat4(&this->dir);
	XMVECTOR up = XMLoadFloat4(&this->up);

	XMMATRIX VM = XMMatrixLookToLH(pos, dir, up);
	XMStoreFloat4x4(&V, VM);

	XMMATRIX PM = XMLoadFloat4x4(&P);
	XMMATRIX ViewProjM = VM * PM;
	XMStoreFloat4x4(&ViewProj, ViewProjM);
}
