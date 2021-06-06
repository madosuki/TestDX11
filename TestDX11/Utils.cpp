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


	if (error != 0) {
		return std::nullopt;
	}



	return std::move(buf);
}