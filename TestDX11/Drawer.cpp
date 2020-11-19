#include "Drawer.h"

DrawerClassNameSpace::Drawer::Drawer()
{

}

DrawerClassNameSpace::Drawer::~Drawer()
{

}

void DrawerClassNameSpace::Drawer::DrawTriAngle(D3DClass* instance)
{
	std::string shader_path_prefix = "C:\\Users\\madosuki\\Documents\\doc_files\\vs_projects\\TestDX11\\TestDX11\\";

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	vertex_shader.Attach(instance->CreateVertexShader(shader_path_prefix + "VertexShader.hlsl", "main"));

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	pixel_shader.Attach(instance->CreatePixelShader(shader_path_prefix + "PixelShader.hlsl", "main"));

	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	D3D11_INPUT_ELEMENT_DESC elements[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	input_layout.Attach(instance->CreateInputLayout(elements, 2, shader_path_prefix + "VertexShader.hlsl", "main"));


	std::vector<DrawerClassNameSpace::Vertex> vertexs =
	{
		{ DirectX::XMFLOAT3(-0.5f,-0.5f,0), DirectX::XMFLOAT4(1,0,0,1)},
		{ DirectX::XMFLOAT3(0.5f,-0.5f,0), DirectX::XMFLOAT4(0,1,0,1)},
		{ DirectX::XMFLOAT3(0.5f, 0.5f,0), DirectX::XMFLOAT4(0,0,1,1)},
		{ DirectX::XMFLOAT3(-0.5f, 0.5f,0), DirectX::XMFLOAT4(0,0,0,1)}
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> vb;
	vb.Attach(instance->CreateVertexBuffer(vertexs.data(), static_cast<UINT>(vertexs.size())));

	std::vector<UINT> idxs = { 0, 1, 2, 0, 2, 3 };
	Microsoft::WRL::ComPtr<ID3D11Buffer> ib;
	// std::reverse(idxs.begin(), idxs.end());
	ib.Attach(instance->CreateIndexBuffer(idxs.data(), static_cast<UINT>(idxs.size())));

	instance->DrawBegin();
	instance->SetVertexShader(vertex_shader.Get());
	instance->SetPixelShader(pixel_shader.Get());
	instance->SetInputLayout(input_layout.Get());
	instance->SetVertexBuffer(vb.Get(), sizeof(ID3D11VertexShader));
	instance->SetIndexBuffer(ib.Get());
	instance->SetRasterizer();

	instance->DrawIndexed(static_cast<UINT>(idxs.size()));

	instance->DrawEnd();
}