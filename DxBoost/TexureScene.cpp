#include "TexureScene.h"
#include<time.h>

TexureScene::TexureScene()
{
}

TexureScene::~TexureScene()
{

}

Scene* TexureScene::createScene(HWND hwnd, ID3D11Device  *d3device, ID3D11DeviceContext *d3deviceContext, ID3D11RenderTargetView  *renderTargetView)
{
	auto scene = new TexureScene();
	scene->d3device = d3device;
	scene->d3deviceContext = d3deviceContext;
	scene->renderTargetView = renderTargetView;
	scene->init();
	scene->hwnd = hwnd;
	return scene;
}

void TexureScene::init()
{
	//³õÊ¼»¯¼ÓÔØ
	//--- ¼ÓÔØ±³¾°Í¼Æ¬
	mTexure = Texure::create(L"bg.png", d3device, d3deviceContext, renderTargetView);
	if (!mTexure)
	{
		MessageBox(NULL, L"¶ÁÈ¡ÎÆÀíÍ¼ÏñÊ§°Ü!", L"ÌáÊ¾", 0);
	}
	
}

void TexureScene::update(float dt)
{

}

void TexureScene::render(IDXGISwapChain* swapChin)
{
	//äÖÈ¾³¡¾°

	d3deviceContext->RSSetState(0);

	

	d3deviceContext->OMSetDepthStencilState(RenderStates::DrawTexureDepthSSt, 0x0);
	mTexure->Render();


	d3deviceContext->RSSetState(0);
	d3deviceContext->OMSetDepthStencilState(0, 0);
}

void TexureScene::Release()
{
	mTexure->Release();
	delete mTexure;
	mTexure = NULL;
	
}

void TexureScene::OnMouseDown(WPARAM btnState, int x, int y)
{
	
}

void TexureScene::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void TexureScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	
}

void TexureScene::setCameraLen(float width, float height)
{
	
}

