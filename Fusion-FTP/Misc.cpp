#include "stdafx.h"
#include "Misc.h"

bool LoadModules()
{
	auto res = sceSysmoduleLoadModuleInternal(SCE_SYSMODULE_INTERNAL_SYSTEM_SERVICE);
	if (res != SCE_OK)
	{
		Logger::Error("%s: Failed to load SCE_SYSMODULE_INTERNAL_SYSTEM_SERVICE (%llX)", __FUNCTION__, res);
		return false;
	}

	res = sceSysmoduleLoadModuleInternal(SCE_SYSMODULE_INTERNAL_NETCTL);
	if (res != 0)
	{
		Logger::Error("%s: Failed to load SCE_SYSMODULE_INTERNAL_NETCTL (%llX)", __FUNCTION__, res);
		return false;
	}

	res = sceSysmoduleLoadModuleInternal(SCE_SYSMODULE_INTERNAL_NET);
	if (res != 0)
	{
		Logger::Error("%s: Failed to load SCE_SYSMODULE_INTERNAL_NET (%llX)", __FUNCTION__, res);
		return false;
	}

	// Start up networking interface
	res = sceNetInit();
	if (res != 0)
	{
		Logger::Error("%s: sceNetInit failed", __FUNCTION__);
		return false;
	}

	res = sceNetCtlInit();
	if (res != 0)
	{
		Logger::Error("%s: sceNetCtlInit failed (%llX)", __FUNCTION__, res);
		return false;
	}

	Logger::Success("%s: Success!", __FUNCTION__);
	return true;
}

std::vector<uint8_t> RecieveData(SceNetId s, size_t maximumLength)
{
	// Create temporary buffer for data.
	auto data = std::vector<uint8_t>(maximumLength);

	if (sceNetRecv(s, data.data(), data.size(), 0) < 0)
		return std::vector<uint8_t>();

	return data;
}

void SendData(SceNetId s, uint8_t* data, size_t len)
{
	sceNetSend(s, data, len, 0);
}

void SendDataCRLF(SceNetId s, std::string data)
{
	data += "\r\n";
	sceNetSend(s, data.data(), data.size(), 0);
}

std::string removeCRLF(const std::string& input)
{
	std::string result = input;
	size_t found = result.find("\r\n");

	// Iterate over the string and remove all occurrences of "\r\n"
	while (found != std::string::npos)
	{
		result.erase(found, 2); // Erase 2 characters (for \r\n)
		found = result.find("\r\n", found);
	}

	return result;
}

std::vector<std::string> TokenizeArgs(const std::string& input)
{
	std::vector<std::string> result;
	std::istringstream iss(input);

	std::string word;
	while (iss >> word)
	{
		result.push_back(word);
	}

	return result;
}

std::string PermissionFromStat(const SceKernelStat& fileStat)
{
	std::string permissions = "";

	// Check file type
	if (S_ISREG(fileStat.st_mode)) permissions += "-";
	else if (S_ISDIR(fileStat.st_mode)) permissions += "d";
	else if (S_ISLNK(fileStat.st_mode)) permissions += "l";
	else if (S_ISCHR(fileStat.st_mode)) permissions += "c";
	else if (S_ISBLK(fileStat.st_mode)) permissions += "b";
	else if (S_ISFIFO(fileStat.st_mode)) permissions += "p";
	else if (S_ISSOCK(fileStat.st_mode)) permissions += "s";
	else permissions += " ";

	// Owner permissions
	permissions += (fileStat.st_mode & S_IRUSR) ? "r" : "-";
	permissions += (fileStat.st_mode & S_IWUSR) ? "w" : "-";
	permissions += (fileStat.st_mode & S_IXUSR) ? "x" : "-";

	// Group permissions
	permissions += (fileStat.st_mode & S_IRGRP) ? "r" : "-";
	permissions += (fileStat.st_mode & S_IWGRP) ? "w" : "-";
	permissions += (fileStat.st_mode & S_IXGRP) ? "x" : "-";

	// Others permissions
	permissions += (fileStat.st_mode & S_IROTH) ? "r" : "-";
	permissions += (fileStat.st_mode & S_IWOTH) ? "w" : "-";
	permissions += (fileStat.st_mode & S_IXOTH) ? "x" : "-";

	return permissions;
}
