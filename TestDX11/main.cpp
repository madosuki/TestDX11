#include <Windows.h>
#include <iostream>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "D3DClass.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 col;
};

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


	std::string shader_path_prefix = "C:\\Users\\madosuki\\Documents\\doc_files\\vs_projects\\TestDX11\\TestDX11\\";

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	vertex_shader.Attach(instance.CreateVertexShader(shader_path_prefix + "VertexShader.hlsl", "main"));

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	pixel_shader.Attach(instance.CreatePixelShader(shader_path_prefix + "PixelShader.hlsl", "main"));

	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	D3D11_INPUT_ELEMENT_DESC elements[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	input_layout.Attach(instance.CreateInputLayout(elements, 2, shader_path_prefix + "VertexShader.hlsl", "main"));


	std::vector<Vertex> vertexs =
	{
		{ DirectX::XMFLOAT3(-0.5f,-0.5f,0), DirectX::XMFLOAT4(1,0,0,1)},
		{ DirectX::XMFLOAT3(0.5f,-0.5f,0), DirectX::XMFLOAT4(0,1,0,1)},
		{ DirectX::XMFLOAT3(0.5f, 0.5f,0), DirectX::XMFLOAT4(0,0,1,1)},
		{ DirectX::XMFLOAT3(-0.5f, 0.5f,0), DirectX::XMFLOAT4(0,0,0,1)}
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> vb;
	vb.Attach(instance.CreateVertexBuffer(vertexs.data(), static_cast<UINT>(vertexs.size())));

	std::vector<UINT> idxs = { 0, 1, 2, 0, 2, 3 };
	Microsoft::WRL::ComPtr<ID3D11Buffer> ib;
	// std::reverse(idxs.begin(), idxs.end());
	ib.Attach(instance.CreateIndexBuffer(idxs.data(), static_cast<UINT>(idxs.size())));

	instance.DrawBegin();
	instance.SetVertexShader(vertex_shader.Get());
	instance.SetPixelShader(pixel_shader.Get());
	instance.SetInputLayout(input_layout.Get());
	instance.SetVertexBuffer(vb.Get(), sizeof(ID3D11VertexShader));
	instance.SetIndexBuffer(ib.Get());
	instance.SetRasterizer(nullptr);

	instance.DrawIndexed(static_cast<UINT>(idxs.size()));

	instance.DrawEnd();

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