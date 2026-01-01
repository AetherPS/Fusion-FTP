#pragma once
#include "FTPClient.h"

class FileClient
{
public:
	static void Delete(CommandArgs);
	static void RemoveDir(CommandArgs);
	static void CreateDir(CommandArgs);
	static void GetFile(CommandArgs);
	static void SendFile(CommandArgs);
	static void RenameFrom(CommandArgs);
	static void RenameTo(CommandArgs);
	static void GetSize(CommandArgs);

private:
	static std::string RenamePath;
};
