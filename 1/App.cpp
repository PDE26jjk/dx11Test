#include "App.h"
#include "GeometryGenerator.h"
#include "vertex.h"
#include "shape.h"
#include "ReflectivePlane.h"
#include "Camera.h"
#include "light.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "BasicEffect.h"
#include "FogEffect.h"
#include "BillBoardEffect.h"
#include "BlurTexFx.h"
#include "WaveFX.h"
#include "hillTess.h"

App::App()
	:
	wnd(800,600,"App?!")
{}
int App::Go()
{
	// ����Ч��
	BasicEffect* pbe = BasicEffect::get_instance(&wnd.Gfx());
	// ��Ч��
	FogEffect Fog(pbe);
	Billboard billBoard(pbe);
	HillTess hillTess(&wnd.Gfx());
	wnd.Gfx().AddEffect(pbe);
	//wnd.Gfx().AddEffect(&hillTess);
	wnd.Gfx().AddEffect(&Fog);
	//wnd.Gfx().AddEffect(&billBoard);

	WaveFX Wave(pbe);
	Wave.Init(200.0f, 200.0f,5.0f,5.0f,10.0f,10.0f);
	wnd.Gfx().AddEffect(&Wave);

	Fog.SetFog(0.0f, 1000.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.1f));


	ReflectivePlane mirror1(
		XMFLOAT3(0.0f, 100.0f, 100.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f),
		2000.0f, 200.0f
	);

	using MeshData = PDEshape::MeshData;

	GeometryGenerator geo;
	PDEshape::MeshData md;

	geo.CreateGrid(300.0f,300.0f,1000,1000, md);
	PDEshape grid(md);
	grid.Scale(5, 0, 5);
	grid.SetShadow(false);

	geo.CreateGeosphere(20.0f, 5, md);
	PDEshape sphere(md);
	sphere.SetBlendType(BlendType::Transparency);
	sphere.Move(20.0f, 20.0f, 00.0f);
	sphere.Scale(6.0f, 4.50f, 9.0f);
	sphere.SetShadow(false);


	PDEshape sphere2(sphere);
	sphere2.Scale(2.0f, .50f, 3.0f);
	sphere2.Move(100.0f, 0.0f, 0.0f);

	geo.CreateCylinder(20.0f, 10.0f, 20.0f, 50, 0, md);
	PDEshape cylinder(md);

	geo.CreateBox(50.0f, 50.0f, 50.0f, md);
	PDEshape box(md);
	box.Scale(2, 2, 2);
	box.Move(0.0f, 50.0f, 0.0f);
	box.SetTexture("misuhara.dds");
	box.SetBlendType(BlendType::Transparency);

	
	PDEshape box2 = box;
	//box.ScaleTex(0.05f, 1.0/6.0f);
	//box2.SetTexture("WoodCrate01.dds");
	box2.SetTexture("WoodCrate02.dds");
	box2.SetMatOpaque(0.5f);
	//box2.SetBlendType(BlendType::Transparency);
	PDEshape box3 = box2;
	box2.Move(-10.0f, 0.0f, 50.0f);
	box2.Scale(2.0f, 2.0f, 2.0f);
	box3.Move(-200.0f, 0.0f, -100.0f);
	box3.SetTexture("misuhara.dds");

	Material wavesMat;
	//mWavesMat.Ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	wavesMat.Ambient = XMFLOAT4(0.99f, 0.99f, 0.99f, 1.0f);
	wavesMat.Diffuse = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	wavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);
	wavesMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	Material ballMat;
	//mWavesMat.Ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	ballMat.Ambient = XMFLOAT4(0.0f, 0.99f, 0.99f, 1.0f);
	ballMat.Diffuse = XMFLOAT4(0.5f, 0.8f, 0.556f, 0.8f);
	ballMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);
	ballMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	grid.SetMaterial(wavesMat);
	sphere.SetMaterial(ballMat);
	sphere2.SetMaterial(ballMat);
	cylinder.SetMaterial(ballMat);

	std::vector<PDEshape*> shapes = {};

	shapes.push_back(&box2);
	//shapes.push_back(&sphere);
	//shapes.push_back(&mirror1);

	pbe->AddShapes(shapes);

	Light DirLight(LightType::DirectionalLight);
	DirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	DirLight.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	DirLight.Specular = XMFLOAT4(0.2f, 0.8f, 0.4f, 1.0f);
	DirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
	pbe->AddLight(DirLight);
	Light DirLight2(LightType::DirectionalLight);
	DirLight2.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	DirLight2.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	DirLight2.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	DirLight2.Direction = XMFLOAT3(-0.57735f, 0.57735f, 0.57735f);
	pbe->AddLight(DirLight2);
	//be.AddLight(DirLight);

	Light SpotLight(LightType::SpotLight);
	SpotLight.Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	SpotLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SpotLight.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	SpotLight.Att = XMFLOAT3(0.0f, 0.003f, 0.0f);
	SpotLight.Spot = 30.0f;
	SpotLight.Range = 100000.0f;
	SpotLight.Position =XMFLOAT3(0.0f, 100.0f, 0.0f);
	SpotLight.Direction =XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	Light SpotLight2 = SpotLight;
	SpotLight2.Spot = 1.0f;
	SpotLight2.Range = 100000.0f;
	SpotLight2.Position = XMFLOAT3(200.0f, 500.0f, 0.0f);
	XMFLOAT3 to = XMFLOAT3(0.0f, 00.0f, 0.0f);

	//SpotLight2.Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);
	XMStoreFloat3(&SpotLight2.Direction, XMVector3Normalize(XMLoadFloat3(&to) - XMLoadFloat3(&SpotLight2.Position)));

	//be.AddLight(SpotLight);
	pbe->AddLight(SpotLight2);

	// �����������
	XMVECTOR pos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR dir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	wnd.Gfx().GetMainCamera().SetCamera(pos,dir);
	// ������ˢ������ɫ
	wnd.Gfx().SetBackgroundColor(XMFLOAT4(102 / 255.0f, 204 / 255.0f, 1.0f,1.0f));
	while (true)
	{
		if (const auto ecode = Window::ProcessMessages()) {
			return *ecode;
		}
		else {
		DoFrame();
		//Sleep(10);
		}
	}
}

void App::DoFrame()
{
	static int x = 0;
	static int y = 0;
	static int tex_num = 0;
	static float speed = 10.0f;
	static Camera *pCamera = &wnd.Gfx().GetMainCamera();
	static Mouse *pMouse = &wnd.mouse;
	static Keyboard *pKbd = &wnd.kbd;
	static Graphics *pGfx = &wnd.Gfx();
	const float t = timer.Mark();

	//static float mTheta = 1.5f * XM_PI;
	//static float mPhi = 0.25f * XM_PI;
	//static float mRadius = 100.0f;
	// ��ʱ����
	std::ostringstream oss;
	oss << std::setprecision(5)<< std::fixed<< t*1000
	<<	"(ms) fps:" << std::setprecision(0) << std::fixed << 1.0f/t;
	oss << " "<< "position" << "("
		<< std::setprecision(3) << std::fixed 
		<< pCamera->GetPosition().m128_f32[0] << ","
		<< pCamera->GetPosition().m128_f32[1] << ","
		<< pCamera->GetPosition().m128_f32[2] << ")";
	oss << " "<< "direction" << "("
		<< pCamera->GetDirection().m128_f32[0] << ","
		<< pCamera->GetDirection().m128_f32[1] << ","
		<< pCamera->GetDirection().m128_f32[2] << ")";
	wnd.SetTitle(oss.str().c_str());

	//wnd.Gfx().ClearBuffer(); //#66ccff
	const float c = sin(timer.Peek()*3) / 2.0f + 0.5f;

	// �������ֲ�x����
	XMVECTOR cameraLx = XMVector3Cross(Coordinate::Y, pCamera->GetDirection());

	// �������
	const auto mouseEvent = wnd.mouse.Read();
	using mouseType = Mouse::Event::Type;
	if ((mouseType)mouseEvent.GetType() == mouseType::Move) {
		float dx = 0.005f * static_cast<float>(x - pMouse->GetPosX());
		float dy = 0.005f * static_cast<float>(y - pMouse->GetPosY());
		x = pMouse->GetPosX();
		y = pMouse->GetPosY();
		if (pMouse->LeftIsPressed()) {
		// �������λ�ú�ƫ��
			//���������
		pCamera->RotateCamera(cameraLx, -dy);
		pCamera->RotateCamera(Coordinate::Y, -dx);
			


		pGfx->UpdateCamera();
		}
	
	}
	
	// �������
	XMVECTOR cameraOffset = XMVectorZero();
	if (pMouse->LeftIsPressed()) {
		if (pKbd->KeyIsPressed('W') && !pKbd->KeyIsPressed('S')) {
			cameraOffset += pCamera->GetDirection() * speed;

		}
		else if (pKbd->KeyIsPressed('S') && !pKbd->KeyIsPressed('W')) {
			cameraOffset -= pCamera->GetDirection() * speed;

		}
		if (pKbd->KeyIsPressed('D') && !pKbd->KeyIsPressed('A')) {
			cameraOffset += cameraLx * speed;

		}
		else if (pKbd->KeyIsPressed('A') && !pKbd->KeyIsPressed('D')) {
			cameraOffset -= cameraLx * speed;

		}
		if (pKbd->KeyIsPressed('Q')) {
			cameraOffset -= Coordinate::Y * speed;
		}
		if (pKbd->KeyIsPressed('E')) {
			cameraOffset += Coordinate::Y * speed;
		}

		if (XMVector3NotEqual(cameraOffset, XMVectorZero())) {
			pCamera->MoveCamera(cameraOffset);
			pGfx->UpdateCamera();
		}
	}
	
	

	//wnd.Gfx().DrawBox();
	//wnd.Gfx().DrawCylinder();
	pGfx->DrawFrame();
	//wnd.Gfx().DrawHills();
	//wnd.Gfx().ClearBuf03fer(c, c, 1.0f);
	//++tex_num;
	//static float du = 0.05f;
	//static float dv = 1.0f/6.0f;

	/*����*/
	//wnd.Gfx().Shapes()[3].MoveTex(du, 0.0f);
	//if (tex_num % 20 == 0) {
	//	wnd.Gfx().Shapes()[3].MoveTex(0.0f, 1.0f*dv);
	//}
	//if (tex_num % 120 == 0) {
	//	wnd.Gfx().Shapes()[3].ResetTex();
	//	wnd.Gfx().Shapes()[3].ScaleTex(0.05f, 1.0 / 6.0f);

	//}
	/*������*/
	pGfx->EndFrame();
}
