#pragma once
#include "D3DUtil.h"
#define window_width 800
#define window_hight 600
class D3DBase
{
public:
	HWND wind_hwnd;
	UINT wind_width;
	UINT wind_hight;
	UINT                    check_4x_msaa;       //释放支持四倍抗锯齿	
	ID3D11Device            *device;       //d3d设备
	ID3D11DeviceContext     *deviceContext;       //设备描述表
	IDXGISwapChain          *mSwapChain;          //交换链信息	
    ID3D11RenderTargetView  *renderTargetView; //视图变量
	ID3D11DepthStencilView  *mDepthStencilView;
	ID3D11DepthStencilState *m_depthStencilState;
	ID3D11Texture2D         *mDepthStencilBuffer;
	D3D11_VIEWPORT           mScreenViewport;
	D3D_DRIVER_TYPE driverType;
	UINT m4xMsaaQuality;
	BOOL mEnable4xMsaa;
	bool                    check_init;          //检验初始化是否成功
	XMMATRIX mWorldMatrix;  //世界变换矩阵
	XMMATRIX mOrthoMatrix;; //正交矩阵
	XMMATRIX mProjMatrix;  //投影矩阵

	

public:
	D3DBase(HWND wind_hwnd,UINT wind_width,UINT wind_hight);
	~D3DBase();
	bool init(HWND wind_hwnd, UINT wind_width, UINT wind_hight);
	virtual void Update(float dt)   = 0;
	virtual void Render()  = 0;
	virtual void Release()  = 0;	
	bool ifsucceed();
	virtual void OnResize();

	XMMATRIX GetWorldMatrix() { return mWorldMatrix; }
	XMMATRIX GetOrthoMatrix() { return mOrthoMatrix; }
	XMMATRIX GetProjMatrix() { return mProjMatrix; };

	ID3D11Device* GetD3Device() { return device; };
	ID3D11DeviceContext* GetD3DeviceContext() { return deviceContext; };
	IDXGISwapChain* GetSwapChain() { return mSwapChain; };
	ID3D11RenderTargetView  *GetRenderTargetView() { return renderTargetView; };
public:		
	template<class T> 
	void Safe_Release(T t)
	{
		if(t != NULL)
		{
			t->Release();
			t = 0;
		}
	}
};