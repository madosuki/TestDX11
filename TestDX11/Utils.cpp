#include "Utils.h"

Utils::Utils() {}
Utils::~Utils() {}

std::optional<std::string> Utils::GetUserDirectory()
{
	// auto profile = wgetenv("LOCALAPPDATA");
    size_t bufSize = 250;
	// auto buf = std::make_shared<wchar_t>(bufSize);
	char* buf = new char[bufSize];
	auto error = _dupenv_s(&buf, &bufSize, "LOCALAPPDATA");


	if (error != 0 || buf == nullptr) {
		return std::nullopt;
	}

	auto result = std::string(buf);
	delete[] buf;

	return std::move(result);
}

std::wstring Utils::StringToWstring(const std::string& target)
{
	int bufSize = MultiByteToWideChar(CP_OEMCP, 0, target.c_str(), -1, (wchar_t*)NULL, 0);
	wchar_t* buf = new wchar_t[bufSize];

	int check = MultiByteToWideChar(CP_OEMCP, 0, target.c_str(), bufSize, buf, bufSize);

	auto result = std::wstring(buf);
	delete[] buf;

	return std::move(result);
}
