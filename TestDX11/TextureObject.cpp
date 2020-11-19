#include "TextureObject.h"

TextureObject::TextureObject()
{
	texture = nullptr;
}
TextureObject::~TextureObject() {}

void TextureObject::SetTexture(const Microsoft::WRL::ComPtr<ID3D11Device>& device, 
	const std::shared_ptr<ImageUtil::ImageObject>& image_object,
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device_context)
{
	Init(device, image_object);
	SetShader(device_context);
}

void TextureObject::Init(const Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::shared_ptr<ImageUtil::ImageObject> &image_object)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = image_object->Width();
	desc.Height = image_object->Height();
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA surface_data;
	surface_data.pSysMem = image_object->Bytes();
	surface_data.SysMemPitch = image_object->Width() * 4;
	surface_data.SysMemSlicePitch = image_object->PixelLength();

	if (device == nullptr)
		return;

	auto hr = device->CreateTexture2D(&desc, &surface_data, &texture);
	if (FAILED(hr))
		return;

	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.MipLODBias = 0.0f;
	sampler_desc.MaxAnisotropy = 1;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;

	for (int i = 0; i < 4; ++i)
		sampler_desc.BorderColor[i] = 0;

	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = device->CreateSamplerState(&sampler_desc, &sampler_state);
	if (FAILED(hr))
		return;


}

void TextureObject::SetShader(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device_context)
{
	device_context->PSSetShaderResources(0, 1, resource_view.GetAddressOf());
}