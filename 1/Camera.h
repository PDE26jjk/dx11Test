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
	//����������꣨�������꣬theta��x����xz�棬phi��y�𡣣�
	float mTheta; float mPhi; float mRadius;
	// ͶӰ���Ӿ���ͶӰ�Ӿ���	
	XMFLOAT4X4 P; XMFLOAT4X4 V; XMFLOAT4X4 ViewProj;
	XMFLOAT3 mEyePosW; XMVECTOR pos; XMVECTOR target;
private:
	XMVECTOR up;
	float AspectRatio;
};