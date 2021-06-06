#include "Drawer.h"

DrawerClassNameSpace::Drawer::Drawer(Utils* utils)
{
	auto f = utils->GetUserDirectory();
	if (!f.has_value())
	{
		OutputDebugString(L"Filed look up appdata folder.");
		return;
	}
	appDataPath = f.value() + "\\TestDX11\\";
	shaderPathPrefix = appDataPath;

	shaderPathMap.insert(std::make_pair("VertexShaderSample", shaderPathPrefix + "VertexShader.hlsl"));
	shaderPathMap.insert(std::make_pair("PixelShaderSample", shaderPathPrefix + "PixelShader.hlsl"));
}

DrawerClassNameSpace::Drawer::~Drawer()
{

}

void DrawerClassNameSpace::Drawer::DrawTriAngle(D3DClass* instance, Utils* utils)
{

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	vertex_shader.Attach(instance->CreateVertexShader(shaderPathMap["VertexShaderSample"], "main"));

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	pixel_shader.Attach(instance->CreatePixelShader(shaderPathMap["PixelShaderSample"], "main"));

	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	D3D11_INPUT_ELEMENT_DESC elements[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	input_layout.Attach(instance->CreateInputLayout(elements, 2, shaderPathMap["VertexShaderSample"], "main"));


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


	auto imagePath = utils->StringToWstring(appDataPath);

	auto image_object = std::make_shared<ImageUtil::ImageObject>();
	image_object->SetImageFromFile(imagePath + L"test.jpg");

	OutputDebugString((L"size: " + std::to_wstring(image_object->PixelLength()) + L"\r\n").c_str());

	
	auto texture_object = TextureObject();
	auto device = instance->DevicePtr();
	auto device_context = instance->DeviceContextPtr();
	texture_object.SetTexture(device, image_object, device_context);
	
	instance->DrawBegin();

	
	instance->SetVertexShader(vertex_shader.Get());
	instance->SetPixelShader(pixel_shader.Get());
	instance->SetInputLayout(input_layout.Get());
	instance->SetVertexBuffer(vb.Get(), sizeof(DrawerClassNameSpace::Vertex));
	instance->SetIndexBuffer(ib.Get());
	instance->SetRasterizer();
	

	instance->DrawIndexed(static_cast<UINT>(idxs.size()));

	instance->DrawEnd();
}