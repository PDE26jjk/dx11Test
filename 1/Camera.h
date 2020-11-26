#pragma once
#include "h.h"
using namespace DirectX;

class Camera {
public:
	Camera();
	Camera(const XMVECTOR& pos, const XMVECTOR& dir, float AspectRatio);
	void SetCamera(const XMVECTOR& pos,const XMVECTOR& dir);
	void MoveCamera(const XMVECTOR& offset);
	void RotateCamera(const XMVECTOR& A, float angle);
	void SetAspectRatio(const float& AspectRatio);
public:
	// 投影、视矩阵、投影视矩阵	
	XMFLOAT4X4 P; XMFLOAT4X4 V; XMFLOAT4X4 ViewProj;
	XMVECTOR const GetDirection() { return XMLoadFloat4(&dir); }
	XMVECTOR const GetPosition() { return XMLoadFloat4(&pos); }
private:
	XMFLOAT4 pos; XMFLOAT4 dir;
	void UpdateViewMatrix();
	XMFLOAT4 up;
	float AspectRatio;
	float FovAngleY;
	float NearZ;
	float FarZ;
};