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
	UINT                    check_4x_msaa;       //�ͷ�֧���ı������	
	ID3D11Device            *device;       //d3d�豸
	ID3D11DeviceContext     *deviceContext;       //�豸������
	IDXGISwapChain          *mSwapChain;          //��������Ϣ	
    ID3D11RenderTargetView  *renderTargetView; //��ͼ����
	ID3D11DepthStencilView  *mDepthStencilView;
	ID3D11DepthStencilState *m_depthStencilState;
	ID3D11Texture2D         *mDepthStencilBuffer;
	D3D11_VIEWPORT           mScreenViewport;
	D3D_DRIVER_TYPE driverType;
	UINT m4xMsaaQuality;
	BOOL mEnable4xMsaa;
	bool                    check_init;          //�����ʼ���Ƿ�ɹ�
	XMMATRIX mWorldMatrix;  //����任����
	XMMATRIX mOrthoMatrix;; //��������
	XMMATRIX mProjMatrix;  //ͶӰ����

	

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