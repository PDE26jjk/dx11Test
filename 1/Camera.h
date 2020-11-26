#pragma once
#include "h.h"
using namespace DirectX;

class Camera {
public:
	Camera();
	Camera(float mTheta, float mPhi, float mRadius,float AspectRatio);
	void SetCamera(float mTheta, float mPhi, float mRadius);
	void SetCamera(const XMVECTOR& pos,const  XMVECTOR& target);
	void MoveCamera(const XMVECTOR& offset);
	void SetAspectRatio(const float& AspectRatio);
public:
	//摄像机球坐标（左手坐标，theta从x起沿xz面，phi从y起。）
	float mTheta; float mPhi; float mRadius;
	// 投影、视矩阵、投影视矩阵	
	XMFLOAT4X4 P; XMFLOAT4X4 V; XMFLOAT4X4 ViewProj;
	XMFLOAT3 mEyePosW; XMVECTOR pos; XMVECTOR target;
private:
	XMVECTOR up;
	float AspectRatio;
};