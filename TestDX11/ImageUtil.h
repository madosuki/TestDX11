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

		const UINT Width()
		{
			return this->width;
		}

		const UINT Height()
		{
			return this->height;
		}

		const UINT PixelLength()
		{
			return this->pixel_bytes_length;
		}

		const BYTE* Bytes()
		{
			return this->bytes;
		}
	};

}