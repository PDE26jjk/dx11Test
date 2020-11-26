#pragma once
#include "Shape.h"
#include "Camera.h"

class ReflectivePlane :public PDEshape {
	friend class Graphics;
	friend class BasicEffect;
	friend class GeometryGenerator;
public:
	ReflectivePlane(XMFLOAT3 Position, XMFLOAT3 Direction,float width, float depth);
	void SetReflectView(ID3D11RenderTargetView* RTview, ID3D11ShaderResourceView* SRview);
	void SetCamera(const XMVECTOR& pos, const  XMVECTOR& target);
protected:
	virtual void init();
private:
	Camera cameraReflect;
	XMMATRIX refWVP;
	XMFLOAT3 Position;
	XMFLOAT3 Direction;
	wrl::ComPtr<ID3D11RenderTargetView> RenderTargetView;
};