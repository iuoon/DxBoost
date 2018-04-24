#pragma once
#pragma once
#include "Engine/D3DUtil.h"
#include "Engine/Texure.h"
#include "Engine/Effects.h"
#include "Engine/Terrain.h"
#include "Engine/Sky.h"
#include "Engine/Camera.h"
#include "Engine/RenderStates.h"
#include "Engine/Vertex.h"
#include "Engine/Scene.h"

class TexureScene :public Scene
{
public:
	TexureScene();
	~TexureScene();

	static Scene* createScene(HWND hwnd, ID3D11Device  *d3device, ID3D11DeviceContext *d3deviceContext, ID3D11RenderTargetView  *renderTargetView);

	void init();

	void update(float dt);

	void render(IDXGISwapChain* swapChin);

	void Release();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	void setCameraLen(float width, float height);
private:
	Texure* mTexure; //������ͼ
	
};

