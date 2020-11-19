#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

struct SecondVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 texture;
};

class TextureObject
{
private:

	ID3D11Texture2D* texture;

	void CreateTexture();

public:
	TextureObject();
	~TextureObject();

};

