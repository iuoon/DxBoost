#pragma once
#include "d3dUtil.h"

class Scene
{
public:
	Scene();
	~Scene();

	virtual void init() {}

	virtual void update(float dt) {}

	virtual void render(IDXGISwapChain* swapChin) {}

	virtual void Release() {}

	virtual void setCameraLen(float width, float height) {};

	virtual void OnMouseDown(WPARAM btnState, int x, int y) {};
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {};
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {};

protected:
	HWND hwnd;
	ID3D11Device  *d3device;
	ID3D11DeviceContext *d3deviceContext;
	ID3D11RenderTargetView  *renderTargetView;
};

