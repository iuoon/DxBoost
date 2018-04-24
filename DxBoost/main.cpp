#include "AppDelegate.h"
#include <WindowsX.h>
#include "Engine/GameTimer.h"
	

class WindowMain
{
protected:
	HWND         hwnd;
	MSG          msg;                                                   //存储消息的结构。
	WNDCLASS     wndclass;
	UINT          viewport_width;
	UINT          viewport_height;
	HINSTANCE    hInstance;
	HINSTANCE    hPrevInstance;
	PSTR         szCmdLine;
	int          iCmdShow;
	AppDelegate *appDelegate;
	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mResizing;
	GameTimer mTimer;
public:
	WindowMain(HINSTANCE hInstance_need, HINSTANCE hPrevInstance_need, PSTR szCmdLine_need, int iCmdShow_need, int width, int height);
	HRESULT game_create();
	HRESULT game_loop();
	WPARAM game_end();
	void OnResize();
   LRESULT  WndProc(HWND, UINT, WPARAM, LPARAM);
   void OnMouseDown(WPARAM btnState, int x, int y);
   void OnMouseUp(WPARAM btnState, int x, int y);
   void OnMouseMove(WPARAM btnState, int x, int y);
};

WindowMain* mApp = NULL;

LRESULT  WindowMain::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;
	case WM_SIZE:
		if (appDelegate)
		{
			appDelegate->wind_width = LOWORD(lParam);
			appDelegate->wind_hight = LOWORD(lParam);
		}
		else {
			return 0;
		}
		if (appDelegate->device)
		{
			
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{

				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}		
		return 0;
	case WM_SYSCOMMAND:	
		switch (wParam & 0xfff0)
		{
		case SC_KEYMENU:
		case SC_MOUSEMENU:
			//拦截第一种操作，不要通知给默认的消息处理函数  
			return 0;
		//case SC_MOVE:
		//	return 0;
		}	    
	    break;
	case WM_NCRBUTTONDOWN:
		//拦截第二种操作，不要通知给默认的消息处理函数  
		return 0;
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		PrintDebug("=====move ^&***");
		OnResize();
		return 0;
	case WM_KEYDOWN:                // 键盘按下消息
		if (wParam == VK_ESCAPE)    // ESC键
			DestroyWindow(hwnd);    // 销毁窗口, 并发送一条WM_DESTROY消息		
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void WindowMain::OnMouseDown(WPARAM btnState, int x, int y)
{
	appDelegate->OnMouseDown(btnState, x, y);
}

void WindowMain::OnMouseUp(WPARAM btnState, int x, int y)
{
	appDelegate->OnMouseUp(btnState, x, y);
}

void WindowMain::OnMouseMove(WPARAM btnState, int x, int y)
{
	appDelegate->OnMouseMove(btnState, x, y);
}

LRESULT  CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return mApp->WndProc(hwnd, message, wParam, lParam);
}



WindowMain::WindowMain(HINSTANCE hInstance_need, HINSTANCE hPrevInstance_need, PSTR szCmdLine_need, int iCmdShow_need, int width, int height):
mAppPaused(false),
mMinimized(false),
mMaximized(false),
mResizing(false)
{
	hInstance = hInstance_need;
	hPrevInstance = hPrevInstance_need;
	szCmdLine = szCmdLine_need;
	iCmdShow = iCmdShow_need;
	viewport_width = width;
	viewport_height = height;
	mApp = this;
}
HRESULT WindowMain::game_create()
{
	wndclass.style = CS_HREDRAW | CS_VREDRAW;                   //窗口类的类型（此处包括竖直与水平平移或者大小改变时时的刷新）。msdn原文介绍：Redraws the entire window if a movement or size adjustment changes the width of the client area.
	wndclass.lpfnWndProc = MainWndProc;                                   //确定窗口的回调函数，当窗口获得windows的回调消息时用于处理消息的函数。
	wndclass.cbClsExtra = 0;                                         //为窗口类末尾分配额外的字节。
	wndclass.cbWndExtra = 0;                                         //为窗口类的实例末尾额外分配的字节。
	wndclass.hInstance = hInstance;                                 //创建该窗口类的窗口的句柄。
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);          //窗口类的图标句柄。
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);              //窗口类的光标句柄。
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);     //窗口类的背景画刷句柄。
	wndclass.lpszMenuName = 0;                                      //窗口类的菜单。
	wndclass.lpszClassName = TEXT("WindowMain");                                 //窗口类的名称。

	if (!RegisterClass(&wndclass))                                      //注册窗口类。
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),TEXT("iuoon_engine"), MB_ICONERROR);
		return E_FAIL;
	}
	RECT R = { 0, 0, window_width, window_hight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	hwnd = CreateWindow(TEXT("WindowMain"), // window class name创建窗口所用的窗口类的名字。
		TEXT("忘川回云"), // window caption所要创建的窗口的标题。
		WS_OVERLAPPEDWINDOW,        // window style所要创建的窗口的类型（这里使用的是一个拥有标准窗口形状的类型，包括了标题，系统菜单，最大化最小化等）。
		CW_USEDEFAULT,              // initial x position窗口的初始位置水平坐标。
		CW_USEDEFAULT,              // initial y position窗口的初始位置垂直坐标。
		width,               // initial x size窗口的水平位置大小。
		height,               // initial y size窗口的垂直位置大小。
		0,                       // parent window handle其父窗口的句柄。
		0,                       // window menu handle其菜单的句柄。
		hInstance,                  // program instance handle窗口程序的实例句柄。
		0);                     // creation parameters创建窗口的指针
	if (hwnd == NULL) 
	{
		return E_FAIL;
	}
	
	ShowWindow(hwnd, SW_SHOW);   // 将窗口显示到桌面上。
	UpdateWindow(hwnd);           // 刷新一遍窗口（直接刷新，不向windows消息循环队列做请示）。
	return true;
}
HRESULT WindowMain::game_loop()
{
	mTimer.Reset();
	//游戏循环
	ZeroMemory(&msg, sizeof(msg));
	appDelegate = new AppDelegate(hwnd, viewport_width, viewport_height, hInstance);
	if (appDelegate->init_create() == S_OK)
	{
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);//消息转换
				DispatchMessage(&msg);//消息传递给窗口过程函数
			}
			else
			{
				mTimer.Tick();

				if (!mAppPaused)
				{
					appDelegate->Update(mTimer.DeltaTime());
					appDelegate->Render();
				}
				else {
					Sleep(100);
				}

			}
		}
		appDelegate->Release();
	}
	
	return msg.wParam;
}
WPARAM WindowMain::game_end()
{
	delete appDelegate;
	appDelegate = NULL;
	return msg.wParam;
}

void WindowMain::OnResize()
{
	appDelegate->OnResize();
}



//windows函数的入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{

	WindowMain gd3dApp(hInstance, hPrevInstance, szCmdLine, iCmdShow, window_width, window_hight);
	gd3dApp.game_create();
	gd3dApp.game_loop();
	return gd3dApp.game_end();
}

