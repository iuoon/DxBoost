#include "D3DApp.h"

void D3DApp::Release()
{
	renderTargetView->Release();
	mSwapChain->Release();
	deviceContext->ClearState();
	deviceContext->Release();
#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug *d3dDebug;
	HRESULT hr = device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
	if (d3dDebug != nullptr)            d3dDebug->Release();
#endif
	if (device != nullptr)            device->Release();

}

D3DApp::D3DApp(HWND hwnd_need, UINT width_need, UINT hight_need, HINSTANCE hInstance_need) :D3DBase(hwnd_need, width_need, hight_need)
{
	hInstance = hInstance_need;
	//游戏时间
	delta_need = 0.0f;
}

HRESULT D3DApp::init_create()
{

	check_init = init(wind_hwnd, wind_width, wind_hight);
	if (check_init == false)
	{
		MessageBox(0, L"create d3dx11 failed", L"tip", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}
void D3DApp::Update(float dt)
{
	//PrintDebug("the application update!!!\n");
}
void D3DApp::Render()
{
	//交换到屏幕
	HRESULT hr = mSwapChain->Present(0, 0);
}

void D3DApp::clear() {
	//清除屏幕
	XMVECTORF32 color = { 0.0f, 0.0f, 0.25f, 1.0f };
//	deviceContext->ClearRenderTargetView(renderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

