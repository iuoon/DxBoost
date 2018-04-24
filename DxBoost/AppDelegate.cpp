#include "AppDelegate.h"


AppDelegate::AppDelegate(HWND hwnd, UINT width_need, UINT hight_need, HINSTANCE hInstance_need) :D3DApp(hwnd, width_need, hight_need, hInstance_need),hwnd(hwnd)
{

}

AppDelegate::~AppDelegate()
{

}

HRESULT AppDelegate::init_create()
{
	HRESULT r = __super::init_create();
	if (r == S_OK)
	{
		this->init();
	}
	return r;
}

void AppDelegate::init()
{
	scene = TexureScene::createScene(hwnd,GetD3Device(), GetD3DeviceContext(),GetRenderTargetView());
}


void AppDelegate::OnResize()
{
	__super::OnResize();
	if (scene)
	{
		scene->setCameraLen(wind_width, wind_hight);
	}
	
}

void AppDelegate::Update(float dt)
{
	scene->update(dt);
	__super::Update(dt);
}

void AppDelegate::Render()
{
	this->clear();
	
	scene->render(GetSwapChain());
	__super::Render();
}

void AppDelegate::clear() {
	__super::clear();
}

void AppDelegate::Release()
{
	scene->Release();
	delete scene;
	scene = NULL;
	__super::Release();
}
