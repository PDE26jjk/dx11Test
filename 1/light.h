#pragma once
#include "h.h"
using namespace DirectX;

struct DirectionalLight
{
	// ma,md,ms
	DirectionalLight() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float Pad; // Pad the last float so we can
	// array of lights if we wanted.
};
struct PointLight
{
	PointLight() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;
	// Packed into 4D vector: (A0, A1, A2, Pad)
	XMFLOAT3 Att;
	float Pad; // Pad the last float so we can set an
	// array of lights if we wanted.
};
struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;// Packed into 4D vector: (Position, Range)
	XMFLOAT3 Position;
	float Range;
	// Packed into 4D vector: (Direction,Spot)
	XMFLOAT3 Direction;
	float Spot;
	// Packed into 4D vector: (Att, Pad)
	XMFLOAT3 Att;
	float Pad; // Pad the last float so we can set an
	// array of lights if we wanted.
};
enum class LightType {
	DirectionalLight,
	PointLight,
	SpotLight,
	OtherLight
};
class Light
{
public:
	Light(LightType type)
		:
		type(type)
	{
		Ambient = {0.0f,0.0f, 0.0f, 1.0f};
		Diffuse = { 0.0f,0.0f, 0.0f, 1.0f };
		Specular = { 0.0f,0.0f, 0.0f, 1.0f };
		Position = { 0.0f,0.0f, 0.0f};
		Range = 1000.0f;
		Direction = { 0.0f,0.0f, 0.0f };
		Spot = 96.0f;
		Att = { 0.0f,0.0f, 1.0f };
		light = nullptr;
	}
	~Light()
	{
		if (light != nullptr) {
			delete light;
		}
	}
	void* getLight() {
		Free();
		if (type == LightType::DirectionalLight) {
			DirectionalLight* dl = new DirectionalLight();
			dl->Ambient = Ambient;
			dl->Diffuse = Diffuse;
			dl->Specular = Specular;
			dl->Direction = Direction;
			light = dl;
		}
		else if (type == LightType::PointLight) {

			PointLight* pl = new PointLight();
			pl->Ambient = Ambient;
			pl->Diffuse = Diffuse;
			pl->Specular = Specular;
			pl->Position = Position;
			pl->Range = Range;
			pl->Att = Att;
			light = pl;
		}
		else if (type == LightType::SpotLight) {
			SpotLight* sl = new SpotLight();
			sl->Ambient = Ambient;
			sl->Diffuse = Diffuse;
			sl->Specular = Specular;
			sl->Position = Position;
			sl->Range = Range;
			sl->Direction = Direction;
			sl->Spot = Spot;
			sl->Att = Att;
			light = sl;
		}
		return light;
	}
	LightType getType() { return type; }
	void ChangeType(LightType type) { this->type = type; };
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Position;
	float Range;
	XMFLOAT3 Direction;
	float Spot;
	XMFLOAT3 Att;
private:
	void Free() {
		if (light != nullptr) {
			delete light;
		}
	}
	LightType type;
	void* light;
};


struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;
};