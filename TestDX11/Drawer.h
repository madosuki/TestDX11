#pragma once

#include "D3DClass.h"
#include "TextureObject.h"
#include "Utils.h"

namespace DrawerClassNameSpace
{
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 col;
	};

	class Drawer
	{
	public:
		Drawer();
		~Drawer();

		void DrawTriAngle(D3DClass* instance);
	};

}