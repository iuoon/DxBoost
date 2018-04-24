#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "Macro.h"
#include "D3DUtil.h"
#include "D3DX11.h"
#include "WNode.h"

class Texure:public WNode
{
public:
	Texure();
	virtual ~Texure();
	
	static Texure* create(LPCTSTR fileName,ID3D11Device  *d3device, ID3D11DeviceContext *d3deviceContext, ID3D11RenderTargetView  *renderTargetView);
	
	void Release();

	void Update(float dt);
	void Render();
private:
	bool LoadContent(LPCTSTR fileName);
	
private:
	ID3D11Device            *device;
	ID3D11DeviceContext     *deviceContext;
	ID3D11RenderTargetView  *mRenderTargetView;

	ID3D11VertexShader* solidColorVS_;
	ID3D11PixelShader* solidColorPS_;

	ID3D11InputLayout* inputLayout_;
	ID3D11Buffer* vertexBuffer_;

	ID3D11ShaderResourceView* colorMap_;
	ID3D11SamplerState* colorMapSampler_;
private:
	int bmpWidth;//Í¼ÏñµÄ¿í  
	int bmpHeight;//Í¼ÏñµÄ¸ß  
};