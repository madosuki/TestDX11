#pragma once


#include <map>

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
		Drawer(Utils* utils);
		~Drawer();

		void DrawTriAngle(D3DClass* instance, Utils* utils);

	private:
		std::string appDataPath;
		std::string shaderPathPrefix;
		std::map<std::string, std::string> shaderPathMap;
	};

}