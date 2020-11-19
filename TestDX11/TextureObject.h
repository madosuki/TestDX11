#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#include "ImageUtil.h"

struct SecondVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
};

class TextureObject
{
private:

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	// ID3D11Texture2D* texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resource_view;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;

	void Init(const Microsoft::WRL::ComPtr<ID3D11Device> &device, const std::shared_ptr<ImageUtil::ImageObject> &image_object);
	void SetShader(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device_context);

public:
	TextureObject();
	~TextureObject();
	void SetTexture(const Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const std::shared_ptr<ImageUtil::ImageObject>& image_object,
		const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& device_context);

};

