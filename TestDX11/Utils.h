#pragma once

#include <UserEnv.h>
#include <string>
#include <optional>
#include <memory>

class Utils
{
public:
	Utils();
	~Utils();

	std::optional<std::string> GetUserDirectory();
	std::wstring StringToWstring(const std::string& target);
};

