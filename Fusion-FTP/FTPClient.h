#pragma once

class FTPClient;
#define CommandArgs FTPClient* inst, SceNetId s, std::string args

class FTPClient
{
public:
	bool IsRunning;
	SceNetId Sock;
	SceNetInAddr Address;
	SceNetInAddr LocalAddr;
	const static std::map<std::string, std::function<void(CommandArgs)>> Commands;

	std::unique_ptr<DirectoryProvider> WorkingDirectory;
	std::unique_ptr<BaseClient> Client;

	FTPClient(SceNetId s, SceNetInAddr addr, SceNetInAddr localAddr);
	~FTPClient();

private:
	static void ChangeWorkingDir(CommandArgs);
	static void Passive(CommandArgs);
	static void Active(CommandArgs);
	static void Type(CommandArgs);
	static void List(CommandArgs);

	void MainLoop();
};
