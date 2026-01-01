#pragma once

class DirectoryProvider
{
public:
	std::string CurrentDirectory = "/";

	bool NavigateUp();
	bool MoveToRelative(std::string relativeDir);
	bool SetAbsoluteDirectory(std::string path);
	std::string GetRelativePath(std::string file);

private:

};
