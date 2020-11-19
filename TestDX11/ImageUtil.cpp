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
	{
		delete bytes;
	}
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
		return;

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder = nullptr;
	hr = factory->CreateDecoderFromFilename(
		filename.c_str(),
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&decoder
	);

	if (FAILED(hr))
		return;

	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame = nullptr;
	hr = decoder->GetFrame(0, &frame);

	if (FAILED(hr))
		return;

	WICPixelFormatGUID guid_pixel_format;

	hr = frame->GetPixelFormat(&guid_pixel_format);
	if (FAILED(hr))
		return;

	UINT puiWidth, puiHeight;
	hr = frame->GetSize(&puiWidth, &puiHeight);

	UINT buffer_size = puiWidth * puiHeight * 3;
    bytes = new BYTE[buffer_size];

	if (bytes == nullptr)
		return;

	hr = frame->CopyPixels(
		NULL,
		puiWidth * 3,
		buffer_size,
		bytes
	);

	this->width = puiWidth;
	this->height = puiHeight;
	this->pixel_bytes_length = buffer_size;

}