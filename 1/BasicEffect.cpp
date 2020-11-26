
#include "BasicEffect.h"
#include "DDSTextureLoader11.h"
#include <algorithm>

BasicEffect* BasicEffect::this_ptr = nullptr;


BasicEffect::BasicEffect(Graphics* gfx)
	:
	num_DL(0),
	num_PL(0),
	num_SL(0),
	lightIsChange(false),
	mIsLoadShapes(false),

	PDEeffect("basic.cso",gfx)
{
	LoadFx(); 
	SetTopoLayout();
}

void BasicEffect::init()
{
	
}

void BasicEffect::LoadFx()
{
	mTechTexture = mFX->GetTechniqueByName("Texture");
	mTech = mTechNoTexture = mFX->GetTechniqueByName("NOTexture");

	mTechReflect = mFX->GetTechniqueByName("Reflect");
	mTechReflectNoMSAA = mFX->GetTechniqueByName("ReflectNoMSAA");
	mTechShadow = mFX->GetTechniqueByName("Shadow");

	mfxWorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	mfxViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	mfxReflectWorldViewProj = mFX->GetVariableByName("gReflectWorldViewProj")->AsMatrix();
	mfxTime = mFX->GetVariableByName("time")->AsScalar();
	mfxWorld = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mfxEyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();

	mfxMaterial = mFX->GetVariableByName("gMaterial");

	mfxDiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxReflectMap = mFX->GetVariableByName("gReflectMap")->AsShaderResource();
	mfxTexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	mfxShadowL = mFX->GetVariableByName("gShadowL")->AsVector();

	mfxDirLights = mFX->GetVariableByName("DLs");
	mfxPointLights = mFX->GetVariableByName("PLs");
	mfxSpotLights = mFX->GetVariableByName("SLs");
	mfxNum_DL = mFX->GetVariableByName("num_DL")->AsScalar();
	mfxNum_PL = mFX->GetVariableByName("num_PL")->AsScalar();
	mfxNum_SL = mFX->GetVariableByName("num_SL")->AsScalar();

}

void BasicEffect::SetTopoLayout()
{
	/****************************定义拓扑、布局**************************/
	//mTech = mTechBillboard;
	mTopoLayout = TopoLayout(TopoLayoutType::TrianglePNT);


	/****************************定义拓扑、布局完**************************/
}



void BasicEffect::Draw()
{
	
	// 更新图形
	if (!mIsLoadShapes) {
		LoadShapes();
	}
	// 绑定设定到管线
	Bind();
	// 更新资源
	//mTech->GetDesc(&techDesc);
	float now = mGfx->GetRanderTime()*1000.0f;
	mfxTime->SetFloat(now);
	if (lightIsChange) {
		if (num_DL > 0)
			mfxDirLights->SetRawValue(&mDirLights[0], 0, sizeof(mDirLights[0]) * num_DL);
		if (num_PL > 0)
			mfxPointLights->SetRawValue(&mPointLights[0], 0, sizeof(mPointLights[0]) * num_PL);
		if (num_SL > 0)
			mfxSpotLights->SetRawValue(&mSpotLights[0], 0, sizeof(mSpotLights[0]) * num_SL);
		mfxNum_DL->SetInt(num_DL);
		mfxNum_PL->SetInt(num_PL);
		mfxNum_SL->SetInt(num_SL);
		lightIsChange = false;
	}
	//if (mFogIsChange) {
	//	mfxFogStart->SetFloat(mFogStart);
	//	mfxFogRange->SetFloat(mFogRange);
	//	mfxFogColor->SetRawValue(&mFogColor, 0, sizeof(mFogColor));
	//	mFogIsChange = false;
	//}
	mfxEyePosW->SetRawValue(&mGfx->GetMainCamera().mEyePosW, 0, sizeof(mGfx->GetMainCamera().mEyePosW));
	
	for (UINT i = 0; i < mirrors.size(); ++i)
	{
		XMVECTOR refPos = XMVector3Reflect(mGfx->GetMainCamera().pos, XMLoadFloat3(&mirrors[i]->Direction));
		XMVECTOR refDir = XMVector3Reflect(mGfx->GetMainCamera().target - mGfx->GetMainCamera().pos, XMLoadFloat3(&mirrors[i]->Direction));
		mirrors[i]->SetCamera(refPos, refPos + refDir);

		// 将物体绘制到镜子上
		DrawShapes(mirrors[i]->RenderTargetView.Get(), mirrors[i]->cameraReflect);
		//
		
		/*ID3D11Resource* TexBuffer;
		mirrors[i]->RenderTargetView->GetResource(&TexBuffer);*/
		//mirrors[i]->ShaderResourceView.Get()->GetResource(&TexBuffer);

		mGfx->ClearBuffer(*mGfx->GetpMainTarget());
		//SaveDDSTextureToFile(pContext.Get(), TexBuffer, L"some.dds");
		//TexBuffer->Release();
	}
	//pContext->RSSetViewports(1, &mGfx->GetMainViewPort());
	//mGfx->GetDeviceContext()->OMSetRenderTargets(1u, mGfx->GetpMainTarget(), mGfx->GetDepthStencilView());

	DrawMirrors(*mGfx->GetpMainTarget(), mGfx->GetMainCamera());
	DrawShapes(*mGfx->GetpMainTarget(), mGfx->GetMainCamera());
	//drawBillboard();
	for (auto m : mirrors)
		mGfx->ClearBuffer(m->RenderTargetView.Get());

}

#include <DirectXTexP.h>
#include <DirectXTex.h>
#include "BlurTexFx.h"
#include "ScreenGrab11.h"
void BasicEffect::LoadShapes()
{
	// 保存图形
	//this->pShapes.insert(pShapes.end(), Shapes.begin(), Shapes.end());
	if (pShapes.size() < 1u) return;
	/***************计算顶点、索引间隔***********************/
	UINT shapesCounts = pShapes.size();
	mIndexCounts = std::vector<UINT>(shapesCounts);
	mVertexCounts = std::vector<UINT>(shapesCounts);
	mIndexCounts[0] = mVertexCounts[0] = 0;
	
	UINT allIndex = 0u;
	UINT allVertex = 0u;
	for (UINT i = 0; i < shapesCounts - 1; ++i) {
		mIndexCounts[i + 1u] = pShapes[i]->meshData.Indices.size() + mIndexCounts[i];
		mVertexCounts[i + 1u] = pShapes[i]->meshData.Vertices.size() + mVertexCounts[i];
		allIndex += pShapes[i]->meshData.Indices.size();
		allVertex += pShapes[i]->meshData.Vertices.size();
	}
	allIndex += pShapes[shapesCounts - 1u]->meshData.Indices.size();
	allVertex += pShapes[shapesCounts - 1u]->meshData.Vertices.size();

	/***************计算顶点、索引间隔完***********************/
	/***************填充顶点、索引数组、创建纹理视图***********************/

	std::vector<ReflectivePlane*> mirrors;
	mVertices = std::vector<Vertex>();
	mIndexes = std::vector<UINT>();
	mHasTex = std::vector<bool>(shapesCounts);
	for (UINT i = 0; i < shapesCounts; ++i) {
		for (UINT j = 0; j < pShapes[i]->meshData.Indices.size(); ++j)
			mIndexes.push_back(pShapes[i]->meshData.Indices[j]);

		for (UINT j = 0; j < pShapes[i]->meshData.Vertices.size(); ++j) {
			Vertex v;
			XMFLOAT3 p = pShapes[i]->meshData.Vertices[j].Position;
			v.Pos = p;
			v.Normal = pShapes[i]->meshData.Vertices[j].Normal;
			v.Tex = pShapes[i]->meshData.Vertices[j].TexC;
			mVertices.push_back(v);

		}
		// 加载纹理到视图
		if (pShapes[i]->Texture) {
			ID3D11Resource* res;
			ID3D11ShaderResourceView* pSRV;
			//DDS_ALPHA_MODE alpha = DDS_ALPHA_MODE::DDS_ALPHA_MODE_STRAIGHT;
			HR(CreateDDSTextureFromFileEx(pDevice.Get(),pContext.Get(),
				CA2W(pShapes[i]->Texture),0u, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
				true,
				&res, &pSRV, 0u));

			pShapes[i]->SetTextureView(pSRV);
			mHasTex[i] = true;
			res->Release();
			pSRV->Release();
			/*blurredTex->Release();
			mBlurredOutputTexUAV->Release();
			mBlurredOutputTexSRV->Release();*/
		}
		// 如果有镜子要提出来
		ReflectivePlane* mirror = nullptr;
		mirror = dynamic_cast<ReflectivePlane*>(pShapes[i]);

		if (mirror != nullptr) {
			mirrors.push_back(mirror);
			mHasTex[i] = true;
		}
	}
	// 加载镜子
	LoadMirrors(mirrors);
	/***************填充顶点、索引数组完***********************/
	/***************定义顶点、索引、相应缓冲并绑定***********************/

	// 创建不变顶点缓冲
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * allVertex;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &mVertices[0];


	HR(pDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	// Create the index buffer
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * allIndex;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &mIndexes[0];

	HR(pDevice->CreateBuffer(&ibd, &iinitData, &mIB));
	mVBstride = sizeof(Vertex);
	mVBoffset = 0;

	/***************定义顶点、索引、相应缓冲并绑定完***********************/
	mIsLoadShapes = true;
}

void BasicEffect::AddShape(PDEshape* pShape)
{
	this->pShapes.push_back(pShape);

	mIsLoadShapes = false;
}

void BasicEffect::AddShapes(std::vector<PDEshape*> shapes)
{
	if (shapes.size() > 0)
		this->pShapes.insert(this->pShapes.end(), shapes.begin(), shapes.end());
	else
		return;
	mIsLoadShapes = false;
}

inline void BasicEffect::LoadMirrors(const std::vector<ReflectivePlane*>& Mirrors)
{
	if (!Mirrors.size()) return;
	this->mirrors = Mirrors;
	D3D11_TEXTURE2D_DESC mirrorsDesc;
	mirrorsDesc.Width = mGfx->GetWinWidth();
	mirrorsDesc.Height = mGfx->GetWinHeight();
	mirrorsDesc.MipLevels = 1;
	mirrorsDesc.ArraySize = 1;
	mirrorsDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Use 4X MSAA?
	if (mGfx->IsUse4xMsaa())
	{
		mirrorsDesc.SampleDesc.Count = 4;
		// m4xMsaaQuality is returned via CheckMultisampleQualityLevels().
		mirrorsDesc.SampleDesc.Quality = mGfx->Get4xMsaaQuality() - 1;

		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	}
	//No MSAA
	else
	{
		mirrorsDesc.SampleDesc.Count = 1;
		mirrorsDesc.SampleDesc.Quality = 0;

		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	}

	mirrorsDesc.Usage = D3D11_USAGE_DEFAULT;
	mirrorsDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	mirrorsDesc.CPUAccessFlags = 0;
	mirrorsDesc.MiscFlags = 0;

	renderTargetViewDesc.Format = mirrorsDesc.Format;
	renderTargetViewDesc.Texture2D.MipSlice = 0;


	shaderResourceViewDesc.Format = mirrorsDesc.Format;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (UINT i = 0; i < mirrors.size(); ++i) {
		wrl::ComPtr<ID3D11Texture2D> mirrorBuffer;
		wrl::ComPtr<ID3D11RenderTargetView> mirrorRTView;
		wrl::ComPtr<ID3D11ShaderResourceView> mirrorSRView;
		pDevice->CreateTexture2D(&mirrorsDesc, nullptr, mirrorBuffer.GetAddressOf());
		HR(pDevice->CreateRenderTargetView(
			mirrorBuffer.Get(),
			&renderTargetViewDesc,
			&mirrorRTView
		));
		HR(pDevice->CreateShaderResourceView(
			mirrorBuffer.Get(),
			&shaderResourceViewDesc,
			&mirrorSRView
		));
		mirrors[i]->SetReflectView(mirrorRTView.Get(), mirrorSRView.Get());
	}

}

bool BasicEffect::AddLight(Light light)
{
	lightIsChange = true;
	if (light.getType() == LightType::DirectionalLight && mDirLights.size() <= num_DL) {
		mDirLights.push_back(*static_cast<DirectionalLight*>(light.getLight()));
		num_DL += 1;
		return true;
	}
	else if (light.getType() == LightType::PointLight && mPointLights.size() <= num_PL) {
		mPointLights.push_back(*static_cast<PointLight*>(light.getLight()));
		num_PL += 1;
		return true;
	}
	else if (light.getType() == LightType::SpotLight && mSpotLights.size() <= num_SL) {
		mSpotLights.push_back(*static_cast<SpotLight*>(light.getLight()));
		num_SL += 1;
		return true;
	}
	return false;
}

void BasicEffect::DrawShapes(ID3D11RenderTargetView* renderTarget, Camera& camera)
{
	
	pContext->OMSetRenderTargets(1u, &renderTarget, mGfx->GetDepthStencilView());
	// 渲染方式
	pContext->RSSetState(mSOLID.Get());
	//pContext->RSSetState(mWIREFRAME.Get());

	// 计算渲染顺序
	std::vector<UINT> drawOrder = SortShapes(camera);

	// 摄像机的VP矩阵
	XMMATRIX ViewProjM = XMLoadFloat4x4(&camera.ViewProj);
	mfxViewProj->SetMatrix(reinterpret_cast<float*>(&ViewProjM));
	/*********************绘制阴影************************************************************/
	XMMATRIX shadowOffsetY = XMMatrixTranslation(0.0f, 0.01f, 0.0f);
	for (UINT& i : drawOrder) {
		if (!pShapes[i]->HasShadow) continue;
		/****************************更新坐标并绘制**************************/
		XMMATRIX ViewProjM = XMLoadFloat4x4(&camera.ViewProj);
		HR(mfxMaterial->SetRawValue(&pShapes[i]->Mat, 0, sizeof(pShapes[i]->Mat)));

		for (DirectionalLight l : mDirLights) {
			XMVECTOR L = XMVectorSet(l.Direction.x, l.Direction.y, l.Direction.z, 0.0f);
			mfxShadowL->SetFloatVector(reinterpret_cast<float*>(&L));

			XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // xz plane
			XMMATRIX S = XMMatrixShadow(shadowPlane, -L);

			XMMATRIX world = XMLoadFloat4x4(&pShapes[i]->W) * S * shadowOffsetY;
			XMMATRIX worldViewProj = world * ViewProjM;

			mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&PDEshape::InverseTranspose(world)));
			mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
			static float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			pContext->OMSetBlendState(mBlendStateTransparency.Get(), blendFactors, 0xffffffff);

			mTech = mTechShadow;
			pContext->OMSetDepthStencilState(mNoDoubleBlendDSS.Get(), 0);

			for (UINT p = 0; p < techDesc.Passes; ++p)
			{
				mTech->GetPassByIndex(p)->Apply(0, pContext.Get());
				pContext->DrawIndexed(pShapes[i]->meshData.Indices.size(), mIndexCounts[i], mVertexCounts[i]);
			}
			/****************************更新坐标并绘制完**************************/
			pContext->RSSetState(0);
		}
	}
	pContext->OMSetDepthStencilState(0, 0);
	//pContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	/*********************绘制阴影完***********************************************************************/

	// Set land material (material varies per object).
	for (UINT& i : drawOrder) {
		if (dynamic_cast<ReflectivePlane*> (pShapes[i])) continue;
		//for (UINT i = 0; i < 2; ++i) {
			/****************************更新坐标并绘制**************************/

		XMMATRIX world = XMLoadFloat4x4(&pShapes[i]->W);
		XMMATRIX worldViewProj = world * ViewProjM;

		mfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
		mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&PDEshape::InverseTranspose(world)));
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

		mfxMaterial->SetRawValue(&pShapes[i]->Mat, 0, sizeof(pShapes[i]->Mat));
		static float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		switch (pShapes[i]->blendType) {
		case BlendType::None:
			pContext->OMSetBlendState(nullptr, blendFactors, 0xffffffff);
			break;
		case BlendType::Add:
			pContext->OMSetBlendState(mBlendStateAdd.Get(), blendFactors, 0xffffffff);
			break;
		case BlendType::Subtract:
			pContext->OMSetBlendState(mBlendStateSubtract.Get(), blendFactors, 0xffffffff);
			break;
		case BlendType::Multiply:
			pContext->OMSetBlendState(mBlendStateMultiply.Get(), blendFactors, 0xffffffff);
			break;
		case BlendType::Transparency:
			pContext->RSSetState(mBACK.Get());
			pContext->OMSetBlendState(mBlendStateTransparency.Get(), blendFactors, 0xffffffff);
			break;
		}
		// 加载贴图
		if (mHasTex[i]) {
			if (pShapes[i]->ShaderResourceView) {
				mTech = mTechTexture;
				HR(mfxDiffuseMap->SetResource(pShapes[i]->ShaderResourceView.Get()));
				//ID3D11Resource* TexBuffer;
				//pShapes[i]->ShaderResourceView.Get()->GetResource(&TexBuffer);
				//D3D11_TEXTURE2D_DESC texDesc;
				//static_cast<ID3D11Texture2D*>(TexBuffer)->GetDesc(&texDesc);
				////mirrors[i]->ShaderResourceView = pMainResourceOut;
				//std::ostringstream oss;
				//oss << "some" << i << ".dds";
				//SaveDDSTextureToFile(pContext.Get(), TexBuffer, CA2W(oss.str().c_str()));

				mfxTexTransform->SetMatrix(reinterpret_cast<float*>(&pShapes[i]->TexW));
			}
		}
		else {
			mTech = mTechNoTexture;
		}
		
		pShapes[i]->GetEffect()->GetDrawTech()->GetDesc(&techDesc);

		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			mTech->GetPassByIndex(p)->Apply(0, pContext.Get());
			pContext->DrawIndexed(pShapes[i]->meshData.Indices.size(), mIndexCounts[i], mVertexCounts[i]);
		}
		/****************************更新坐标并绘制完**************************/
		pContext->RSSetState(0);
	}
}

inline void BasicEffect::DrawMirrors(ID3D11RenderTargetView* target, Camera& camera)
{
	pContext->OMSetRenderTargets(1u, &target, mGfx->GetDepthStencilView());
	// 渲染方式
	pContext->RSSetState(mSOLID.Get());
	//pContext->RSSetState(mWIREFRAME.Get());

	std::vector<PDEshape*>& mirrors = pShapes;
	// Set land material (material varies per object).
	for (UINT i = 0; i < mirrors.size(); ++i) {
		if (!dynamic_cast<ReflectivePlane*> (mirrors[i])) continue;
		/****************************更新坐标并绘制**************************/
		XMMATRIX ViewProjM = XMLoadFloat4x4(&camera.ViewProj);
		XMMATRIX world = XMLoadFloat4x4(&mirrors[i]->W);
		XMMATRIX worldViewProj = world * ViewProjM;

		mfxWorld->SetMatrix(reinterpret_cast<float*>(&world));
		mfxWorldInvTranspose->SetMatrix(reinterpret_cast<float*>(&PDEshape::InverseTranspose(world)));
		mfxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&worldViewProj));

		mfxReflectWorldViewProj->SetMatrix(reinterpret_cast<float*>(&dynamic_cast<ReflectivePlane*>(mirrors[i])->refWVP));

		mfxMaterial->SetRawValue(&mirrors[i]->Mat, 0, sizeof(mirrors[i]->Mat));
		static float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		//pContext->RSSetState(mBACK.Get());
		pContext->OMSetBlendState(mBlendStateTransparency.Get(), blendFactors, 0xffffffff);

		if (mGfx->IsUse4xMsaa()) {
			mTech = mTechReflect;
			HR(mfxReflectMap->SetResource(mirrors[i]->ShaderResourceView.Get()));

		}
		else {
			mTech = mTechReflectNoMSAA;
			HR(mfxDiffuseMap->SetResource(mirrors[i]->ShaderResourceView.Get()));
		}

		mfxTexTransform->SetMatrix(reinterpret_cast<float*>(&mirrors[i]->TexW));
		
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			mTech->GetPassByIndex(p)->Apply(0, pContext.Get());
			pContext->DrawIndexed(mirrors[i]->meshData.Indices.size(), mIndexCounts[i], mVertexCounts[i]);
		}
		/****************************更新坐标并绘制完**************************/
		pContext->RSSetState(0);
	}
}

std::vector<UINT>& BasicEffect::SortShapes(Camera& camera) const
{
	static bool isGroup = false;
	static XMVECTOR targeToPos = camera.target - camera.pos;
	static std::vector<UINT> order(pShapes.size());
	if (!pShapes.size())
		return order;

	static UINT TPstart = 0; // 透明开始索引
	if (!isGroup) {
		for (UINT i = 0; i < pShapes.size(); ++i) {
			order[i] = i;
		}
		for (UINT i = 0; i < pShapes.size() - 1; ++i) {
			if (pShapes[order[i]]->blendType == BlendType::None) continue;

			//if (pShapes[order[i]]->blendType == BlendType::Transparency && order[i] < pShapes.size()-1) {
			if (order[i] < pShapes.size() - 1) {
				UINT j = i + 1;
				while (pShapes[order[j]]->blendType != BlendType::None) {
					++j;
					if (j >= order.size()) goto out;
				}
				std::swap(order[j], order[i]);
				TPstart = i + 1;
			}
		}
	out:
		isGroup = true;
	}

	// 定义按到摄像机距离比大小函数,根据向量与标量夹角为锐角时，向量头距离标量尾更远，反则反是
	// 只是将全局矩阵的w(局部坐标原点)当做图形的位置，较为粗略，适用条件是
	// 1，图形顺序和其局部坐标原点顺序一致，
	// 2, 同一个图形的像素在视点到投影面上的路径上是连续的，即图形间无重叠和穿插
	auto sortByDistanceToCamera = [=](UINT& a, UINT& b)->bool {
		XMVECTOR AB = (XMLoadFloat4x4(&pShapes[a]->W) - XMLoadFloat4x4(&pShapes[b]->W)).r[3];
		return XMVectorGetX(XMVector3Dot(AB, targeToPos)) > 0u;
	};


	// 透明图形按到摄像机距离排序
	std::sort(order.begin() + TPstart, order.end(), sortByDistanceToCamera);
	if (TPstart > 2)
		sort(order.begin() + TPstart - 1, order.begin(), sortByDistanceToCamera);
	return order;
}