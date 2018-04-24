#include"D3DBase.h"

D3DBase::D3DBase(HWND hwnd_need, UINT width_need, UINT hight_need):mDepthStencilBuffer(0), mDepthStencilView(0), m4xMsaaQuality(0), mEnable4xMsaa(false)
{
	device = NULL;
	deviceContext = NULL;
	mSwapChain = NULL;
	renderTargetView = NULL;
	wind_hwnd  = hwnd_need;
	wind_width = width_need;
	wind_hight = hight_need;
}
bool D3DBase::init(HWND hwnd_need, UINT width_need, UINT hight_need)
{
	UINT create_flag = 0;
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE
	};

	unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);

	driverType = driverTypes[0];

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	unsigned int totalFeatureLevels = ARRAYSIZE(featureLevels);


	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,                 // default adapter
		driverType,
		0,                 // no software device
		createDeviceFlags,
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		&device,
		&featureLevel,
		&deviceContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	device->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
	assert(m4xMsaaQuality > 0);

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = width_need;
	sd.BufferDesc.Height = hight_need;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// Use 4X MSAA? 
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hwnd_need;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;


	IDXGIDevice* dxgiDevice = 0;
	device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

	IDXGIAdapter* dxgiAdapter = 0;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);

	IDXGIFactory* dxgiFactory = 0;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	dxgiFactory->CreateSwapChain(device, &sd, &mSwapChain);

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	/*D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(200);
	viewport.Height = static_cast<float>(80);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;*/

//	deviceContext->RSSetViewports(1, &viewport);
	//-------------------------------------------------------------
	//第十一,创建投影矩阵，初始化世界矩阵和用户接口矩阵
	//-------------------------------------------------------------
	float fieldOfView, screenAspect;
	float ScreenNear, ScreenDepth;
	fieldOfView = XM_PIDIV2;
	screenAspect = (float)wind_width / (float)wind_hight;
	ScreenNear = 1;
	ScreenDepth = 100;
	//投影矩阵
	mProjMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, ScreenNear, ScreenDepth);
	//世界矩阵
	mWorldMatrix = XMMatrixIdentity();
	//2D渲染矩阵,用于变换用户接口,正交投影矩阵
	mOrthoMatrix = XMMatrixOrthographicLH(static_cast<float>(wind_width), static_cast<float>(wind_hight), ScreenNear, ScreenDepth);

	OnResize();

	check_init = true;
	return true;
}
D3DBase::~D3DBase()
{
	if (deviceContext)
		deviceContext->ClearState();
	Safe_Release(device);
	Safe_Release(deviceContext);
	Safe_Release(mSwapChain);
	Safe_Release(renderTargetView);
	Safe_Release(mDepthStencilView);
	Safe_Release(mDepthStencilBuffer);
}
bool D3DBase::ifsucceed()
{
	return check_init;
}

void D3DBase::OnResize()
{
	assert(deviceContext);
	assert(device);
	assert(mSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	Safe_Release(renderTargetView);
	Safe_Release(mDepthStencilView);
	Safe_Release(mDepthStencilBuffer);


	// Resize the swap chain and recreate the render target view.

	mSwapChain->ResizeBuffers(1, wind_width, wind_hight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	ID3D11Texture2D* backBuffer;
	mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	device->CreateRenderTargetView(backBuffer, 0, &renderTargetView);
	Safe_Release(backBuffer);

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = wind_width;
	depthStencilDesc.Height = wind_hight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	device->CreateTexture2D(&depthStencilDesc, NULL, &mDepthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = depthStencilDesc.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(mDepthStencilBuffer, &descDSV, &mDepthStencilView);

	// Bind the render target view and depth/stencil view to the pipeline.

	deviceContext->OMSetRenderTargets(1, &renderTargetView, mDepthStencilView);
	

	// Set the viewport transform.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(wind_width);
	mScreenViewport.Height = static_cast<float>(wind_hight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	deviceContext->RSSetViewports(1, &mScreenViewport);
}

