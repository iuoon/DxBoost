#pragma once
#include "D3DApp.h"
#include "TestScene.h"
#include "TexureScene.h"

class AppDelegate :public D3DApp
{
public:
	AppDelegate(HWND hwnd_need, UINT width_need, UINT hight_need, HINSTANCE hInstance_need);
	~AppDelegate();

	virtual HRESULT init_create();
	virtual void Update(float dt);
	virtual void Render();
	virtual void clear();
	virtual void Release();

	void OnMouseDown(WPARAM btnState, int x, int y) { scene->OnMouseDown(btnState,x,y); }
	void OnMouseUp(WPARAM btnState, int x, int y) { scene->OnMouseUp(btnState, x, y); }
	void OnMouseMove(WPARAM btnState, int x, int y) { scene->OnMouseMove(btnState, x, y); }

	void init();

    void OnResize();

private:
	HWND hwnd;
	Scene* scene;
};

