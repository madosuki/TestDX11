#pragma once

#include <Windows.h>
#include <iostream>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

class D3DClass
{
private:
	Microsoft::WRL::ComPtr<ID3D11Device> device_ptr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_ptr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain_ptr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	D3D11_VIEWPORT viewport;

	HWND hwnd;

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

	/*
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
	*/

	HRESULT Init(HWND hWnd);

	ID3D11VertexShader* CreateVertexShader(const std::string& filename, const std::string& entrypath = "", bool error = true);

	ID3D11PixelShader* CreatePixelShader(const std::string& filename, const std::string& entrypath = "", bool error = true);

	ID3D11InputLayout* CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* layout, UINT elemnt_number, const std::string& filename, const std::string& entrypath = "");

	void SetInputLayout(ID3D11InputLayout* vertex_layout);
	void SetVertexShader(ID3D11VertexShader* vertex_shader);
	void SetPixelShader(ID3D11PixelShader* pixel_shader);

	void SetVertexBuffer(ID3D11Buffer* vertex_buffer, UINT buffer_size);
	void SetIndexBuffer(ID3D11Buffer* index_buffer);

	void SetRasterizer(ID3D11RasterizerState* state = nullptr);

	void SetTexture2D(UINT register_number, ID3D11ShaderResourceView* texture);

	void DrawBegin();
	void DrawEnd();
	void DrawIndexed(UINT vertex_number);

	Microsoft::WRL::ComPtr<ID3D11Device> DevicePtr() { return this->device_ptr; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContextPtr() { return this->context_ptr; }


	template<class x>
	ID3D11Buffer* CreateVertexBuffer(x* VertexData, UINT VertexNum)
	{
		//頂点バッファ作成
		D3D11_BUFFER_DESC hBufferDesc;
		ZeroMemory(&hBufferDesc, sizeof(hBufferDesc));
		hBufferDesc.ByteWidth = sizeof(x) * VertexNum;
		hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		hBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		hBufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA hSubResourceData;
		ZeroMemory(&hSubResourceData, sizeof(hSubResourceData));
		hSubResourceData.pSysMem = VertexData;

		ID3D11Buffer* hpBuffer;
		if (FAILED(device_ptr->CreateBuffer(&hBufferDesc, &hSubResourceData, &hpBuffer))) {
			return nullptr;
		}
		return hpBuffer;
	}
	ID3D11Buffer* CreateIndexBuffer(UINT* Index, UINT IndexNum)
	{
		//インデックスバッファ作成
		D3D11_BUFFER_DESC hBufferDesc;
		ZeroMemory(&hBufferDesc, sizeof(hBufferDesc));
		hBufferDesc.ByteWidth = sizeof(UINT) * IndexNum;
		hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		hBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		hBufferDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA hSubResourceData;
		ZeroMemory(&hSubResourceData, sizeof(hSubResourceData));
		hSubResourceData.pSysMem = Index;

		ID3D11Buffer* hpBuffer;
		if (FAILED(device_ptr->CreateBuffer(&hBufferDesc, &hSubResourceData, &hpBuffer))) {
			return nullptr;
		}
		return hpBuffer;
	}


};

