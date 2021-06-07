#include "ImageUtil.h"

void ImageUtil::ImageObject::LoadJpegFile(const std::wstring& filename)
{
	auto c = Utils();
	unsigned char* dstBytes = nullptr;
	size_t dstBytesSize = 0;
	int jpgWidth = 0;
	int jpgHeight = 0;
	auto fpath = c.WstringToString(filename, CP_OEMCP);
	if (fpath.has_value())
	{
		FILE* fp;
		int err = fopen_s(&fp, fpath.value().c_str(), "rb");
		if (fp != nullptr) {
			int fd = _fileno(fp);
			struct _stat s;
			err = _fstat(fd, &s);
			size_t fileSize = s.st_size;
			auto buf = new unsigned char[fileSize];
			auto readCount = fread_s(buf, fileSize, 1, fileSize, fp);
			fclose(fp);

			wchar_t wstBuf[1024];
			wsprintfW(wstBuf, L"size: %lu\r\n", readCount);
			OutputDebugStringW(wstBuf);

			tjhandle handle = tjInitDecompress();
			int jpegSubSamp = 0;
			tjDecompressHeader2(handle, buf, fileSize, &jpgWidth, &jpgHeight, &jpegSubSamp);

			size_t size = (size_t)jpgWidth * (size_t)jpgHeight * 3;
			dstBytes = new unsigned char[size];
			tjDecompress2(handle, buf, fileSize, dstBytes, jpgWidth, 0, jpgHeight, TJPF_RGB, TJFLAG_FASTDCT);
			dstBytesSize = size;
			tjDestroy(handle);
			delete[] buf;

			bytes = dstBytes;
			width = jpgWidth;
			height = jpgHeight;
			pixel_bytes_length = size;
		}
	}
}

ImageUtil::ImageObject::ImageObject()
{
	bytes = nullptr;
	width = 0;
	height = 0;
	pixel_bytes_length = 0;
}

ImageUtil::ImageObject::~ImageObject()
{
	if (bytes != nullptr)
		delete[] bytes;
}

void ImageUtil::ImageObject::SetImageFromFile(const std::wstring& filename)
{
	Microsoft::WRL::ComPtr<IWICImagingFactory> factory = nullptr;
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&factory
	);

	if (FAILED(hr))
	{
		OutputDebugString(L"Failed CoCreateInstance\r\n");
		return;
	}

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder = nullptr;
	hr = factory->CreateDecoderFromFilename(
		filename.c_str(),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&decoder
	);

	if (FAILED(hr))
	{
		OutputDebugStringW((filename + L"\r\n").c_str());
		OutputDebugStringW(L"Failed CreateDecodeFromFilename\r\n");
		return;
	}

	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame = nullptr;
	hr = decoder->GetFrame(0, &frame);

	if (FAILED(hr))
	{
		OutputDebugStringW(L"Failed GetFrame\r\n");
		return;
	}

	WICPixelFormatGUID guid_pixel_format;
	hr = frame->GetPixelFormat(&guid_pixel_format);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"Failed GetPixelFormat");
		return;
	}
	
	
	UINT puiWidth, puiHeight;
	hr = frame->GetSize(&puiWidth, &puiHeight);
	if (hr != S_OK) {
		OutputDebugStringW(L"Failed GetSize in ImageUtil.cpp");
		return;
	}
	OutputDebugStringW((std::to_wstring(puiWidth) + L", " + std::to_wstring(puiHeight) + L"\r\n").c_str());

	UINT buffer_size = puiWidth * puiHeight * 3;
	bytes = new BYTE[buffer_size];
	if (bytes == nullptr)
	{
		OutputDebugStringW(L"Failed init bytes\r\n");
		return;
	}



	hr = frame->CopyPixels(
		NULL,
		puiWidth * 3,
		buffer_size,
		bytes
	);

	if (FAILED(hr))
	{
		OutputDebugStringW(L"Failed CopyPixels\r\n");
		return;
	}

	this->width = puiWidth;
	this->height = puiHeight;
	this->pixel_bytes_length = buffer_size;
}