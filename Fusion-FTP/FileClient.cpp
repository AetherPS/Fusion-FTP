#include "stdafx.h"
#include "FileClient.h"

std::string FileClient::RenamePath;

void FileClient::Delete(CommandArgs)
{
	if (sceKernelUnlink(inst->WorkingDirectory->GetRelativePath(args).c_str()) != 0)
	{
		SendDataCRLF(s, "550 Could not delete the file.");
		return;
	}

	SendDataCRLF(s, "226 File deleted.");
}

void FileClient::RemoveDir(CommandArgs)
{
	if (sceKernelRmdir(inst->WorkingDirectory->GetRelativePath(args).c_str()) != 0)
	{
		SendDataCRLF(s, "550 Could not delete the directory.");
		return;
	}

	SendDataCRLF(s, "226 Directory deleted.");
}

void FileClient::CreateDir(CommandArgs)
{
	if (sceKernelMkdir(inst->WorkingDirectory->GetRelativePath(args).c_str(), 0777) != 0)
	{
		SendDataCRLF(s, "550 Could not delete the directory.");
		return;
	}

	SendDataCRLF(s, "226 Directory deleted.");
}

void FileClient::GetFile(CommandArgs)
{
	auto fd = sceKernelOpen(inst->WorkingDirectory->GetRelativePath(args).c_str(), O_RDONLY, 0);
	if (fd < 0)
	{
		SendDataCRLF(s, "550 File not found.");
		return;
	}

	// Set seek if restore point set.
	sceKernelLseek(fd, 0, SEEK_SET);

	// Allocate some temp storage.
	auto buffer = std::make_unique<uint8_t[]>(8192);

	// Connect to the data socket.
	inst->Client->Connect();
	SendDataCRLF(s, "150 Opening data connection.");

	// Recursively read the data in chunks till there is nothing left.
	auto dataRead = 0;
	while ((dataRead = sceKernelRead(fd, buffer.get(), 8192)) > 0)
	{
		if (!inst->Client->SendData(buffer.get(), dataRead))
		{
			Logger::Info("Abort");
			break;
		}
	}

	sceKernelClose(fd);
	inst->Client.reset();

	SendDataCRLF(s, "226 Transfer completed.");
}

void FileClient::SendFile(CommandArgs)
{
	auto fd = sceKernelOpen(inst->WorkingDirectory->GetRelativePath(args).c_str(), O_CREAT | O_RDWR | O_TRUNC, 0777);
	if (fd < 0)
	{
		SendDataCRLF(s, "550 File not found.");
		return;
	}

	// Set seek if restore point set.
	sceKernelLseek(fd, 0, SEEK_SET);

	// Allocate some temp storage.
	auto buffer = std::make_unique<uint8_t[]>(8192);

	// Connect to the data socket.
	inst->Client->Connect();
	SendDataCRLF(s, "150 Opening data connection.");

	auto dataRecieved = 0;
	while ((dataRecieved = inst->Client->RecieveData(buffer.get(), 8192)) > 0)
	{
		if (sceKernelWrite(fd, buffer.get(), dataRecieved) < 0)
		{
			Logger::Error("File write failed.");
			break;
		}
	}

	sceKernelClose(fd);
	inst->Client.reset();

	SendDataCRLF(s, "226 Transfer completed.");
}

void FileClient::RenameFrom(CommandArgs)
{
	auto tempPath = inst->WorkingDirectory->GetRelativePath(args);

	if (sceKernelCheckReachability(tempPath.c_str()) != 0)
	{
		SendDataCRLF(s, "550 File does not exist.");
		return;
	}

	RenamePath = tempPath;
	SendDataCRLF(s, "350 Success waiting for new name.");
}

void FileClient::RenameTo(CommandArgs)
{
	if (sceKernelRename(RenamePath.c_str(), inst->WorkingDirectory->GetRelativePath(args).c_str()) != 0)
	{
		SendDataCRLF(s, "550 Failed to rename.");
		return;
	}

	SendDataCRLF(s, "226 Rename successful.");
}

void FileClient::GetSize(CommandArgs)
{
	SceKernelStat stat;
	if (sceKernelStat(inst->WorkingDirectory->GetRelativePath(args).c_str(), &stat) != 0)
	{
		SendDataCRLF(s, "550 File does not exist.");
		return;
	}

	SendDataCRLF(s, va("213 %lld", stat.st_size));
}