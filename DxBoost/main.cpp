#include "AppDelegate.h"
#include <WindowsX.h>
#include "Engine/GameTimer.h"
	

class WindowMain
{
protected:
	HWND         hwnd;
	MSG          msg;                                                   //�洢��Ϣ�Ľṹ��
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
			//���ص�һ�ֲ�������Ҫ֪ͨ��Ĭ�ϵ���Ϣ������  
			return 0;
		//case SC_MOVE:
		//	return 0;
		}	    
	    break;
	case WM_NCRBUTTONDOWN:
		//���صڶ��ֲ�������Ҫ֪ͨ��Ĭ�ϵ���Ϣ������  
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
	case WM_KEYDOWN:                // ���̰�����Ϣ
		if (wParam == VK_ESCAPE)    // ESC��
			DestroyWindow(hwnd);    // ���ٴ���, ������һ��WM_DESTROY��Ϣ		
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
	wndclass.style = CS_HREDRAW | CS_VREDRAW;                   //����������ͣ��˴�������ֱ��ˮƽƽ�ƻ��ߴ�С�ı�ʱʱ��ˢ�£���msdnԭ�Ľ��ܣ�Redraws the entire window if a movement or size adjustment changes the width of the client area.
	wndclass.lpfnWndProc = MainWndProc;                                   //ȷ�����ڵĻص������������ڻ��windows�Ļص���Ϣʱ���ڴ�����Ϣ�ĺ�����
	wndclass.cbClsExtra = 0;                                         //Ϊ������ĩβ���������ֽڡ�
	wndclass.cbWndExtra = 0;                                         //Ϊ�������ʵ��ĩβ���������ֽڡ�
	wndclass.hInstance = hInstance;                                 //�����ô�����Ĵ��ڵľ����
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);          //�������ͼ������
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);              //������Ĺ������
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);     //������ı�����ˢ�����
	wndclass.lpszMenuName = 0;                                      //������Ĳ˵���
	wndclass.lpszClassName = TEXT("WindowMain");                                 //����������ơ�

	if (!RegisterClass(&wndclass))                                      //ע�ᴰ���ࡣ
	{
		MessageBox(NULL, TEXT("This program requires Windows NT!"),TEXT("iuoon_engine"), MB_ICONERROR);
		return E_FAIL;
	}
	RECT R = { 0, 0, window_width, window_hight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	hwnd = CreateWindow(TEXT("WindowMain"), // window class name�����������õĴ���������֡�
		TEXT("��������"), // window caption��Ҫ�����Ĵ��ڵı��⡣
		WS_OVERLAPPEDWINDOW,        // window style��Ҫ�����Ĵ��ڵ����ͣ�����ʹ�õ���һ��ӵ�б�׼������״�����ͣ������˱��⣬ϵͳ�˵��������С���ȣ���
		CW_USEDEFAULT,              // initial x position���ڵĳ�ʼλ��ˮƽ���ꡣ
		CW_USEDEFAULT,              // initial y position���ڵĳ�ʼλ�ô�ֱ���ꡣ
		width,               // initial x size���ڵ�ˮƽλ�ô�С��
		height,               // initial y size���ڵĴ�ֱλ�ô�С��
		0,                       // parent window handle�丸���ڵľ����
		0,                       // window menu handle��˵��ľ����
		hInstance,                  // program instance handle���ڳ����ʵ�������
		0);                     // creation parameters�������ڵ�ָ��
	if (hwnd == NULL) 
	{
		return E_FAIL;
	}
	
	ShowWindow(hwnd, SW_SHOW);   // ��������ʾ�������ϡ�
	UpdateWindow(hwnd);           // ˢ��һ�鴰�ڣ�ֱ��ˢ�£�����windows��Ϣѭ����������ʾ����
	return true;
}
HRESULT WindowMain::game_loop()
{
	mTimer.Reset();
	//��Ϸѭ��
	ZeroMemory(&msg, sizeof(msg));
	appDelegate = new AppDelegate(hwnd, viewport_width, viewport_height, hInstance);
	if (appDelegate->init_create() == S_OK)
	{
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);//��Ϣת��
				DispatchMessage(&msg);//��Ϣ���ݸ����ڹ��̺���
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



//windows���������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{

	WindowMain gd3dApp(hInstance, hPrevInstance, szCmdLine, iCmdShow, window_width, window_hight);
	gd3dApp.game_create();
	gd3dApp.game_loop();
	return gd3dApp.game_end();
}

