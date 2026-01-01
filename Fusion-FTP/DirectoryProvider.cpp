#include "stdafx.h"
#include "DirectoryProvider.h"

bool DirectoryProvider::NavigateUp()
{
	if (CurrentDirectory.length() == 1 && CurrentDirectory == "/")
	{
		Logger::Error("Cant navigate up any more. We are at the root directory.");
		return false;
	}

	auto tempDir = std::string("/");
	if (CurrentDirectory.find_last_of('/') != 0)
		tempDir = CurrentDirectory.substr(0, CurrentDirectory.find_last_of('/'));

	return SetAbsoluteDirectory(tempDir);
}

bool DirectoryProvider::MoveToRelative(std::string relativeDir)
{
	auto tempDir = std::string("");
	if (CurrentDirectory.length() == 1 && CurrentDirectory == std::string("/"))
		tempDir = va("/%s", relativeDir.c_str());
	else
		tempDir = va("%s/%s", CurrentDirectory.c_str(), relativeDir.c_str());

	return SetAbsoluteDirectory(tempDir);
}

bool DirectoryProvider::SetAbsoluteDirectory(std::string path)
{
	// Make sure the dir is valid.
	if (sceKernelCheckReachability(path.c_str()) != 0)
	{
		Logger::Error("SetAbsoluteDirectory: Path %s is not valid.", path.c_str());
		return false;
	}

	// Update the working dir.
	CurrentDirectory = path;

	return true;
}

std::string DirectoryProvider::GetRelativePath(std::string file)
{
	return va("%s/%s", CurrentDirectory.c_str(), file.c_str());
}