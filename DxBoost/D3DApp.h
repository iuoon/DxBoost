#pragma once
#include "Engine/D3DBase.h"
//¼Ì³ÐµÄd3d×¢²áÀà
class D3DApp :public D3DBase
{
	float                    delta_need;
	HINSTANCE                hInstance;
public:
	D3DApp(HWND wind_hwnd, UINT wind_width, UINT wind_hight, HINSTANCE hInstance);
	HRESULT init_create();
	void Update(float dt);
	void Render();
	void clear();
	void Release();

	

	float AspectRatio() const
	{
		return static_cast<float>(wind_width) / wind_hight;
	}
};
