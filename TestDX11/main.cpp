#include <Windows.h>
#include <iostream>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <string>


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class D3DClass
{
public:
	D3DClass()
	{
		this->device_ptr = nullptr;
		this->context_ptr = nullptr;
		this->swapchain_ptr = nullptr;
		this->texture = nullptr;
		this->render_target_view = nullptr;

		this->viewport = { 0, 0, 0, 0, 0, 0 };

		this->hwnd = NULL;
	}

	~D3DClass() {}
	
	void DevicePtr(ID3D11Device* device)
	{
		this->device_ptr = device;
	}

	ID3D11Device* DevicePtr()
	{
		return this->device_ptr;
	}

	void ContextPtr(ID3D11DeviceContext* context)
	{
		this->context_ptr = context;
	}

	ID3D11DeviceContext* ContextPtr()
	{
		return this->context_ptr;
	}

	HRESULT Init(HWND hWnd);

private:
	ID3D11Device* device_ptr;
	ID3D11DeviceContext* context_ptr;
	IDXGISwapChain* swapchain_ptr;
	ID3D11Texture2D* texture;
	ID3D11RenderTargetView* render_target_view;
	D3D11_VIEWPORT viewport;

	HWND hwnd;
};

HRESULT D3DClass::Init(HWND hWnd)
{
	hwnd = hWnd;

	IDXGIFactory* factory = nullptr;

	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(hr))
		return hr;

	int GPUNumber = 0;
	int GPUMaxMem = 0;
	DXGI_ADAPTER_DESC adapter_desc;
	size_t string_length;
	IDXGIOutput* adapter_output = nullptr;
	uint32_t number_modes = 0;
	IDXGIAdapter* adapter = nullptr;

	for (int i = 0; i < 100; i++)
	{
		IDXGIAdapter* tmp_adapter;
		hr = factory->EnumAdapters(i, &tmp_adapter);
		if (FAILED(hr))
			break;

		hr = tmp_adapter->GetDesc(&adapter_desc);

		char video_card_description[128];
		int error = wcstombs_s(&string_length, video_card_description, 128, adapter_desc.Description, 128);
		if (error != 0)
			break;

		std::string tmp = video_card_description;
		int size = MultiByteToWideChar(CP_UTF8, 0, &tmp[0], (int)tmp.size(), NULL, 0);
		std::wstring wstrTo(size, 0);
		MultiByteToWideChar(CP_UTF8, 0, &tmp[0], (int)tmp.size(), &wstrTo[0], size);

		std::wstring video_card_name_wstring = L"video card name: " + wstrTo + L"\r\n";
		OutputDebugString(video_card_name_wstring.c_str());
		

		int video_card_memory = (int)(adapter_desc.DedicatedVideoMemory / 1024 / 1024);
		std::wstring video_card_memory_wstring = L"video memory: " + std::to_wstring(video_card_memory) + L"\r\n";
		OutputDebugString(video_card_memory_wstring.c_str());

		hr = tmp_adapter->EnumOutputs(0, &adapter_output);
		if (FAILED(hr))
			continue;

		hr = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &number_modes, NULL);
		if (FAILED(hr))
			continue;

		if (video_card_memory > GPUMaxMem)
		{
			GPUMaxMem = video_card_memory;
			GPUNumber = 1;
		}

		tmp_adapter->Release();
		adapter_output->Release();
		adapter_output = nullptr;
	}

	hr = factory->EnumAdapters(GPUNumber, &adapter);
	if (FAILED(hr))
		return hr;

	UINT cdev_flags = 0;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = 1024;
	sd.BufferDesc.Height = 768;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL feature_level_list[] = { D3D_FEATURE_LEVEL_11_1 };

	hr = D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 
		cdev_flags, feature_level_list, 1, D3D11_SDK_VERSION, &sd,
		&swapchain_ptr, &device_ptr, NULL, &context_ptr);

	if (FAILED(hr))
		return hr;

	adapter->Release();
	adapter = nullptr;

	factory->Release();
	factory = nullptr;

	hr = this->swapchain_ptr->GetBuffer(0, IID_PPV_ARGS(&this->texture));
	if (FAILED(hr))
		return hr;

	hr = this->device_ptr->CreateRenderTargetView(texture, NULL, &this->render_target_view);
	if (FAILED(hr))
		return hr;

	viewport.Width = static_cast<FLOAT>(1024 - 0);
	viewport.Height = static_cast<FLOAT>(768 - 0);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	UpdateWindow(hWnd);

	return hr;
}


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
		DefWindowProc(hWnd, message, wParam, lParam);
	}
}