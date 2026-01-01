#include "stdafx.h"
#include "FTPClient.h"
#include <stdio.h>
#include <unistd.h>

const std::map<std::string, std::function<void(CommandArgs)>> FTPClient::Commands =
{
	{ "SYST", [](CommandArgs) { SendDataCRLF(s, "215 UNIX Type: L8"); }},
	{ "AUTH", [](CommandArgs) { SendDataCRLF(s, "534 Sorry, TLS/SSL security is not available."); }},		// Do not support TLS/SSL.
	{ "USER", [](CommandArgs) { SendDataCRLF(s, "331 Username Good."); }},									// Allow any username login.							
	{ "PASS", [](CommandArgs) { SendDataCRLF(s, "230 Login Success!"); }},									// Allow any password login.
	{ "PWD",  [](CommandArgs) { SendDataCRLF(s, va("257 \"%s\" Current working dir.", inst->WorkingDirectory->CurrentDirectory.c_str())); }},
	{ "CWD",  ChangeWorkingDir },
	{ "CDUP", [](CommandArgs) { inst->WorkingDirectory->NavigateUp(); SendDataCRLF(s,"200 ok"); }},
	{ "PASV", Passive },
	{ "PORT", Active },
	{ "TYPE", Type },
	{ "LIST", List },
	{ "DELE", FileClient::Delete },
	{ "RMD",  FileClient::RemoveDir },
	{ "MKD",  FileClient::CreateDir },
	{ "RETR", FileClient::GetFile },
	{ "STOR", FileClient::SendFile },
	{ "RNFR", FileClient::RenameFrom },
	{ "RNTO", FileClient::RenameTo },
	{ "SIZE", FileClient::GetSize },

	// APPE - The client provides the file name it wishes to use for the upload. 
	//		  If the file already exists on the server, the data is appended to the existing file. 
	//		  If the file does not exist, it is created. 
	// 
	// 
	// REST - Specify a marker of where to resume a file transfer later.
	// FEAT - Specify the extended features we support.
};

void FTPClient::ChangeWorkingDir(CommandArgs)
{
	auto result = false;
	if (args == "..")
		result = inst->WorkingDirectory->NavigateUp();
	else if (BeginsWith(args, "/"))
		result = inst->WorkingDirectory->SetAbsoluteDirectory(args);
	else
		result = inst->WorkingDirectory->MoveToRelative(args);

	SendDataCRLF(s, result ? "250 Change working dir ok." : "550 Invalid Directory.");
}

void FTPClient::Passive(CommandArgs)
{
	inst->Client = std::make_unique<PassiveClient>(s, inst->LocalAddr);
}

void FTPClient::Active(CommandArgs)
{
	inst->Client = std::make_unique<ActiveClient>(s, args);
}

void FTPClient::Type(CommandArgs)
{
	switch (args.c_str()[0])
	{
	case 'A':
	case 'I':
		SendDataCRLF(s, "200 Success");
		break;

	default:
		SendDataCRLF(s, "504 Failure");
		break;
	}
}

void FTPClient::List(CommandArgs)
{
	auto tempDirectory = inst->WorkingDirectory->CurrentDirectory;
	if (args.size() > 1)
	{
		Logger::Info("List has dir arg: %s", args.c_str());
		tempDirectory = args[1];
	}

	char buf[16384];
	SceKernelDirent* entry;

	int fd = sceKernelOpen(tempDirectory.data(), O_RDONLY, 0);
	if (fd < 0)
	{
		Logger::Error("sceKernelOpen failed: 0x%08X", fd);
		SendDataCRLF(s, "550 Invalid directory.");

		return;
	}

	memset(buf, 0, sizeof(buf));

	int ret = sceKernelGetdents(fd, buf, sizeof(buf));
	if (ret < 0)
	{
		sceKernelClose(fd);

		Logger::Error("sceKernelGetdents failed: 0x%08X", ret);
		SendDataCRLF(s, "550 Invalid directory.");

		return;
	}

	SendDataCRLF(s, "150 Opening ASCII mode data transfer for LIST.");

	inst->Client->Connect();

	entry = (SceKernelDirent*)buf;
	while (entry->d_fileno != 0)
	{
		SceKernelStat st;
		auto file = va("%s/%s", tempDirectory.c_str(), entry->d_name);

		if (sceKernelStat(file.data(), &st) != 0)
		{
			goto continueList;
		}

		{
			char timeStr[20];
			std::tm* ptm = std::gmtime(&st.st_ctime);
			std::strftime(timeStr, sizeof(timeStr), "%b %d %H:%M", ptm);

			std::string line;
			line += PermissionFromStat(st);
			line += " 1 ps4 iyrtyg ";
			line += std::to_string(st.st_size);
			line += " " + std::string(timeStr);
			line += " " + std::string(entry->d_name);

			inst->Client->SendMessage(line);
		}

continueList:
		entry = (SceKernelDirent*)((char*)entry + entry->d_reclen);
	}

	sceKernelClose(fd);
	inst->Client.reset();

	SendDataCRLF(s, "226 Transfer complete.");
}

void FTPClient::MainLoop()
{
	SendDataCRLF(Sock, "220 Welcome friend to Orbis FTP :)");

	while (IsRunning)
	{
		auto data = RecieveData(Sock, 1024);

		if (data.size() <= 0)
			break;

		auto fullArgs = removeCRLF(std::string(data.begin(), data.end()));
		auto args = TokenizeArgs(fullArgs);

		// Find the command in the map
		auto it = std::find_if(Commands.begin(), Commands.end(), [=](const std::pair<std::string, std::function<void(CommandArgs)>>& pair)
		{
			return strstr(pair.first.c_str(), args[0].c_str());
		});

		// Check if the command exists in the map
		if (it != Commands.end())
		{
			auto firstArgLength = args[0].length() + 1;

			// Call the command function with the given argument
			it->second(this, Sock, firstArgLength >= fullArgs.length() ? "" : fullArgs.substr(firstArgLength, fullArgs.length() - firstArgLength));
		}
		else
		{
			SendDataCRLF(Sock, "502 Command not implemented.");
			Logger::Error("[FTP] Command %s is not implemented at this time.", args[0].c_str());
		}
	}

	IsRunning = false;
}

FTPClient::FTPClient(SceNetId s, SceNetInAddr addr, SceNetInAddr localAddr)
{
	Sock = s;
	Address = addr;
	LocalAddr = localAddr;
	IsRunning = true;
	WorkingDirectory = std::make_unique<DirectoryProvider>();

	// Run the main loop.
	MainLoop();
}

FTPClient::~FTPClient()
{
	IsRunning = false;
}