#include "D3DClass.h"

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

ID3D11VertexShader* D3DClass::CreateVertexShader(const std::string& filename, const std::string& entrypath, bool error)
{
	UINT compile_flags = 0;

	setlocale(LC_CTYPE, "jpn");
	wchar_t ws[512];
	size_t ws_size;
	mbstowcs_s(&ws_size, ws, 512, filename.c_str(), _TRUNCATE);

	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	Microsoft::WRL::ComPtr<ID3DBlob> error_blob;

	HRESULT hr = D3DCompileFromFile(ws, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypath.c_str(), "vs_5_0", compile_flags, 0, &blob, &error_blob);
	if (FAILED(hr))
		return nullptr;

	ID3D11VertexShader* vertex_shader;
	hr = device_ptr->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &vertex_shader);

	if (FAILED(hr))
		return nullptr;

	return vertex_shader;
}

ID3D11PixelShader* D3DClass::CreatePixelShader(const std::string& filename, const std::string& entrypath, bool error)
{
	UINT compile_flags = 0;

	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	Microsoft::WRL::ComPtr<ID3DBlob> blob_error;

	setlocale(LC_CTYPE, "jpn");
	wchar_t ws[512];
	size_t ws_size;
	mbstowcs_s(&ws_size, ws, 512, filename.c_str(), _TRUNCATE);

	auto hr = D3DCompileFromFile(ws, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypath.c_str(), "ps_5_0", compile_flags, 0, &blob, &blob_error);
	if (FAILED(hr))
		return nullptr;

	ID3D11PixelShader* pixel_shader;
	hr = device_ptr->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &pixel_shader);

	if (FAILED(hr))
		return nullptr;

	return pixel_shader;
}

ID3D11InputLayout* D3DClass::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* layout, UINT element_number, const std::string& filename, const std::string& entrypath)
{
	UINT compile_flags = 0;

	Microsoft::WRL::ComPtr<ID3DBlob> blob;
	Microsoft::WRL::ComPtr<ID3DBlob> blob_error;

	setlocale(LC_CTYPE, "jpn");
	wchar_t ws[512];
	size_t ws_size;
	mbstowcs_s(&ws_size, ws, 512, filename.c_str(), _TRUNCATE);

	auto hr = D3DCompileFromFile(ws, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypath.c_str(), "vs_5_0", compile_flags, 0, &blob, &blob_error);
	if (FAILED(hr))
		return nullptr;

	ID3D11InputLayout* vertex_layout_ptr;
	hr = device_ptr->CreateInputLayout(layout, element_number, blob->GetBufferPointer(), blob->GetBufferSize(), &vertex_layout_ptr);

	if (FAILED(hr))
		return nullptr;

	return vertex_layout_ptr;
}

void D3DClass::SetInputLayout(ID3D11InputLayout* vertex_layout)
{
	context_ptr->IASetInputLayout(vertex_layout);
}

void D3DClass::SetVertexShader(ID3D11VertexShader* vertex_shader)
{
	context_ptr->VSSetShader(vertex_shader, nullptr, 0);
}

void D3DClass::SetPixelShader(ID3D11PixelShader* pixel_shader)
{
	context_ptr->PSSetShader(pixel_shader, nullptr, 0);
}

void D3DClass::SetVertexBuffer(ID3D11Buffer* vertex_buffer, UINT buffer_size)
{
	UINT hOffsets = 0;
	context_ptr->IASetVertexBuffers(0, 1, &vertex_buffer, &buffer_size, &hOffsets);
}

void D3DClass::SetIndexBuffer(ID3D11Buffer* index_buffer)
{
	context_ptr->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
}

void D3DClass::SetRasterizer(ID3D11RasterizerState* state)
{
	if (state == nullptr)
	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.CullMode = D3D11_CULL_FRONT;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.FrontCounterClockwise = false;
		// desc.ScissorEnable = false;
		desc.MultisampleEnable = true;


		ID3D11RasterizerState* tmp_state;
		auto hr = device_ptr->CreateRasterizerState(&desc, &tmp_state);

		context_ptr->RSSetState(tmp_state);
	}
	else
	{
		context_ptr->RSSetState(state);
	}
}

void D3DClass::SetTexture2D(UINT register_number, ID3D11ShaderResourceView* shader_resouce_view)
{
	context_ptr->PSSetShaderResources(register_number, 1, &shader_resouce_view);


}

void D3DClass::DrawBegin()
{
	context_ptr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// red, green, blue, alpha
	float clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	context_ptr->ClearRenderTargetView(render_target_view, clear_color);
	context_ptr->RSSetViewports(1, &viewport);

	ID3D11RenderTargetView* rtv[1] = { render_target_view };
	context_ptr->OMSetRenderTargets(1, rtv, nullptr);
}

void D3DClass::DrawEnd()
{
	swapchain_ptr->Present(0, 0);
}

void D3DClass::DrawIndexed(UINT vertex_number)
{
	context_ptr->DrawIndexed(vertex_number, 0, 0);
}

