#include "ImageUtil.h"

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
		delete bytes;

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

	UINT buffer_size = puiWidth * puiHeight * 3;
	bytes = new BYTE[buffer_size];

	if (bytes == nullptr)
	{
		OutputDebugStringW(L"Failed init bytes\r\n");
		return;
	}

	hr = frame->CopyPixels(
		NULL,
		puiWidth * 4,
		buffer_size,
		bytes
	);

	if (FAILED(hr))
	{
		OutputDebugStringW(L"Failed CopyPixels");
		return;
	}

	this->width = puiWidth;
	this->height = puiHeight;
	this->pixel_bytes_length = buffer_size;



}