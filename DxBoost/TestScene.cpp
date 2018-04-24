#include "TestScene.h"
#include<time.h>

TestScene::TestScene():mSky(0), mWalkCamMode(false)
{
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mCam.SetPosition(0.0f, 2.0f, 100.0f);

	mDirLights[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(-0.57735f, -0.57735f, -0.57735f);
}

TestScene::~TestScene()
{
	SafeDelete(mSky);
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

Scene* TestScene::createScene(HWND hwnd,ID3D11Device  *d3device, ID3D11DeviceContext *d3deviceContext, ID3D11RenderTargetView  *renderTargetView)
{
	auto scene = new TestScene();
	scene->d3device = d3device;
	scene->d3deviceContext = d3deviceContext;
	scene->renderTargetView = renderTargetView;
	scene->init();
	scene->hwnd = hwnd;
	return scene;
}

void TestScene::init()
{
	//初始化加载
	//--- 加载背景图片
	mTexure = Texure::create(L"bg.png", d3device, d3deviceContext,renderTargetView);
	if (!mTexure)
	{
		MessageBox(NULL, L"读取纹理图像失败!", L"提示", 0);
	}
	int begintime, endtime;
	//加载天空地形
	begintime = clock();  //计时开始 	
	Effects::InitAll(d3device);
	endtime = clock();
//	PrintDebug("init effect time====%d \n",(endtime-begintime));
	begintime = clock();
	InputLayouts::InitAll(d3device);
	endtime = clock();
//	PrintDebug("init InputLayouts time====%d \n", (endtime - begintime));
	RenderStates::InitAll(d3device);

	mSky = new Sky(d3device, L"Textures/grasscube1024.dds", 5000.0f);

	Terrain::InitInfo tii;
	tii.HeightMapFilename = L"Textures/terrain.raw";
	tii.LayerMapFilename0 = L"Textures/grass.dds";
	tii.LayerMapFilename1 = L"Textures/darkdirt.dds";
	tii.LayerMapFilename2 = L"Textures/stone.dds";
	tii.LayerMapFilename3 = L"Textures/lightdirt.dds";
	tii.LayerMapFilename4 = L"Textures/snow.dds";
	tii.BlendMapFilename = L"Textures/blend.dds";
	tii.HeightScale = 50.0f;
	tii.HeightmapWidth = 2049;
	tii.HeightmapHeight = 2049;
	tii.CellSpacing = 0.5f;
	begintime = clock();
	mTerrain.Init(d3device, d3deviceContext, tii);	
	endtime = clock();
	PrintDebug("init mTerrain time====%d ms\n", (endtime - begintime));
}

void TestScene::update(float dt)
{
	if (GetAsyncKeyState('W') & 0x8000)
		mCam.Walk(10.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mCam.Walk(-10.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mCam.Strafe(-10.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mCam.Strafe(10.0f*dt);

	//
	// Walk/fly mode
	//
	if (GetAsyncKeyState('2') & 0x8000)
		mWalkCamMode = true;
	if (GetAsyncKeyState('3') & 0x8000)
		mWalkCamMode = false;

	// 
	// Clamp camera to terrain surface in walk mode.
	//
	if (mWalkCamMode)
	{
		XMFLOAT3 camPos = mCam.GetPosition();
		float y = mTerrain.GetHeight(camPos.x, camPos.z);
		mCam.SetPosition(camPos.x, y + 2.0f, camPos.z);
	}

	mCam.UpdateViewMatrix();
}

void TestScene::render(IDXGISwapChain* swapChin)
{
	//渲染场景
	

//	d3deviceContext->IASetInputLayout(InputLayouts::Basic32);
//	d3deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
//	if (GetAsyncKeyState('1') & 0x8000)
//		d3deviceContext->RSSetState(RenderStates::WireframeRS);
	mTerrain.Draw(d3deviceContext, mCam, mDirLights);

	d3deviceContext->RSSetState(0);

	mSky->Draw(d3deviceContext, mCam);

	// restore default states, as the SkyFX changes them in the effect file.
	//d3deviceContext->RSSetState(0);
	//d3deviceContext->OMSetDepthStencilState(0, 0);

	d3deviceContext->OMSetDepthStencilState(RenderStates::DrawTexureDepthSSt, 0x0);
	mTexure->Render();
	

	d3deviceContext->RSSetState(0);
	d3deviceContext->OMSetDepthStencilState(0, 0);
}

void TestScene::Release()
{
	mTexure->Release();
	delete mTexure;
	mTexure = NULL;
	SafeDelete(mSky);
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
}

void TestScene::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	SetCapture(hwnd);
}

void TestScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void TestScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mCam.Pitch(dy);
		mCam.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void TestScene::setCameraLen(float width, float height)
{
	mCam.SetLens(0.25f*MathHelper::Pi, width / height, 1.0f, 3000.0f);
}

