#include "Texure.h"


struct VertexPos               //结构体
{
	XMFLOAT3 pos;
	XMFLOAT2 tex0;
};


Texure::Texure(): solidColorVS_(0), solidColorPS_(0),
inputLayout_(0), vertexBuffer_(0),
colorMap_(0), colorMapSampler_(0)
{
	device = NULL;
	deviceContext = NULL;
}


Texure::~Texure()
{

}


Texure * Texure::create(LPCTSTR fileName, ID3D11Device * d3device, ID3D11DeviceContext * d3deviceContext, ID3D11RenderTargetView  *renderTargetView)
{
	Texure* tex = new Texure();
	tex->device = d3device;
	tex->deviceContext = d3deviceContext;
	tex->mRenderTargetView = renderTargetView;
	tex->LoadContent(fileName);
	return tex;
}


bool Texure::LoadContent(LPCTSTR fileName)
{
	ID3DBlob* vsBuffer = 0;

	bool compileResult = CompileD3DShader(L"TextureMap.fx", "VS_Main", "vs_5_0", &vsBuffer);

	if (compileResult == false)
	{
		MessageBox(NULL, L"编译顶点着色器失败!",L"提示",0);
		return false;
	}

	HRESULT d3dResult;

	d3dResult = device->CreateVertexShader(vsBuffer->GetBufferPointer(),
		vsBuffer->GetBufferSize(), 0, &solidColorVS_);

	if (FAILED(d3dResult))
	{
		//DXTRACE_MSG("!");
		MessageBox(0, L"创建顶点着色器失败!", L"提示", 0);
		if (vsBuffer)
			vsBuffer->Release();

		return false;
	}

	D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	unsigned int totalLayoutElements = ARRAYSIZE(solidColorLayout);

	d3dResult = device->CreateInputLayout(solidColorLayout, totalLayoutElements,
		vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &inputLayout_);

	vsBuffer->Release();

	if (FAILED(d3dResult))
	{
		MessageBox(NULL, L"创建输入布局失败!", L"提示", NULL);
		return false;
	}

	ID3DBlob* psBuffer = 0;

	compileResult = CompileD3DShader(L"TextureMap.fx", "PS_Main", "ps_5_0", &psBuffer);

	if (compileResult == false)
	{
		MessageBox(NULL, L"像素着色器编译失败!", L"提示", NULL);
		return false;
	}

	d3dResult = device->CreatePixelShader(psBuffer->GetBufferPointer(),
		psBuffer->GetBufferSize(), 0, &solidColorPS_);

	psBuffer->Release();

	if (FAILED(d3dResult))
	{
		MessageBox(NULL, L"创建像素着色器失败!", L"提示", NULL);
		return false;
	}

	VertexPos vertices[] =
	{
		{ XMFLOAT3(1.0f,  1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f,  1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f,  1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = sizeof(VertexPos) * 6;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(resourceData));
	resourceData.pSysMem = vertices;

	d3dResult = device->CreateBuffer(&vertexDesc, &resourceData, &vertexBuffer_);

	if (FAILED(d3dResult))
	{
		MessageBox(NULL, L"创建顶点缓存失败!", L"提示", NULL);
		return false;
	}

	d3dResult = D3DX11CreateShaderResourceViewFromFile(device, fileName, 0, 0, &colorMap_, 0);
	
	if (FAILED(d3dResult))
	{
		MessageBox(NULL, L"读取纹理图像失败!", L"提示", 0);
		return false;
	}
	//获取图片分辨率
	ID3D11Texture2D* tex2d;
	colorMap_->GetResource((ID3D11Resource**)&tex2d);
	D3D11_TEXTURE2D_DESC texdesc;
	tex2d->GetDesc(&texdesc);
	bmpWidth = texdesc.Width;
	bmpHeight = texdesc.Height;
	tex2d->Release();

	D3D11_SAMPLER_DESC colorMapDesc;
	ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
	colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

	d3dResult = device->CreateSamplerState(&colorMapDesc, &colorMapSampler_);

	if (FAILED(d3dResult))
	{
		MessageBox(NULL, L"创建颜色映射采样器声明失败!", L"提示", NULL);
		return false;
	}
	
	/*D3D11_TEXTURE2D_DESC dsDesc;
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsDesc.Width = 300;
	dsDesc.Height = 200;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsDesc.MipLevels = 1;
	dsDesc.ArraySize = 1;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.MiscFlags = 0;
	dsDesc.Usage = D3D11_USAGE_DEFAULT;
	dsDesc.SampleDesc.Count = 4;
	dsDesc.SampleDesc.Quality = 0;

	ID3D11Texture2D* depthStencilBuffer;
	device->CreateTexture2D(&dsDesc, 0, &depthStencilBuffer);

	ID3D11DepthStencilView  *depthStencilView = 0;
	device->CreateDepthStencilView(depthStencilBuffer, 0, &depthStencilView);
	deviceContext->OMSetRenderTargets(1, &mRenderTargetView, depthStencilView);
	depthStencilBuffer->Release();
	depthStencilView->Release();*/
	return true;
}


void Texure::Release()
{
	if (colorMapSampler_) colorMapSampler_->Release();
	if (colorMap_) colorMap_->Release();
	if (solidColorVS_) solidColorVS_->Release();
	if (solidColorPS_) solidColorPS_->Release();
	if (inputLayout_) inputLayout_->Release();
	if (vertexBuffer_) vertexBuffer_->Release();

	colorMapSampler_ = 0;
	colorMap_ = 0;
	solidColorVS_ = 0;
	solidColorPS_ = 0;
	inputLayout_ = 0;
	vertexBuffer_ = 0;
}

void Texure::Update(float dt)
{
	// 无需进行更新
}


void Texure::Render()       //Render函数的书写
{
//	if (deviceContext == 0)
//		return;
	unsigned int stride = sizeof(VertexPos);
	unsigned int offset = 0;
	deviceContext->IASetInputLayout(inputLayout_);
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->VSSetShader(solidColorVS_, 0, 0);
	deviceContext->PSSetShader(solidColorPS_, 0, 0);
	deviceContext->PSSetShaderResources(0, 1, &colorMap_);
	deviceContext->PSSetSamplers(0, 1, &colorMapSampler_);
	deviceContext->Draw(6, 0);
}