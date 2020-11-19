#include <Windows.h>
#include <iostream>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "D3DClass.h"
#include "Drawer.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreveInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// MessageBox(NULL, TEXT("Hello"), TEXT("Message Window"), MB_OK);

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC };

	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = TEXT("MainWindow");
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (FAILED(RegisterClassEx(&wc)))
	{
		MessageBoxW(NULL, TEXT("error"), TEXT("Failed regist wc"), MB_OK);
		return E_FAIL;
	}

	DWORD style = WS_VISIBLE | WS_DLGFRAME |  WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;

	HWND hwnd = CreateWindow(wc.lpszClassName,
		TEXT("Title"),
		style,
		0, 0,
		1024, 768,
		NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
		return E_FAIL;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	D3DClass instance;

	auto result = instance.Init(hwnd);
	if (FAILED(result))
		OutputDebugStringW(TEXT("Failed Init\r\n"));
	else
		OutputDebugStringW(L"Success Init\r\n");

	auto drawer = DrawerClassNameSpace::Drawer();
	drawer.DrawTriAngle(&instance);


	MSG msg = {0};

	while (WM_QUIT != msg.message)
	{
		if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:

		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}