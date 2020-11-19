#pragma once

#include <memory>
#include <string>
#include <vector>
#include <wrl/client.h>

#include <wincodec.h>

namespace ImageUtil
{

	class ImageObject
	{
	private:
		UINT width;
		UINT height;
		UINT pixel_bytes_length;
		BYTE* bytes;

	public:
		ImageObject();
		~ImageObject();

		void SetImageFromFile(const std::wstring& filename);
	};

}