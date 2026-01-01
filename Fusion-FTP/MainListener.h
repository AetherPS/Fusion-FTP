#pragma once
#include "FTPClient.h"

class MainListener
{
public:
	static void Init();
	static void Term();

private:
	static std::unique_ptr<SocketListener> Listener;
	static std::vector<std::unique_ptr<FTPClient>> Clients;
	static int FtpPort;
	static char IPAddress[16];
	static SceNetInAddr LocalAddr;

	static void ListenerCallback(void* tdParam, SceNetId s, SceNetInAddr sin_addr);
	static void SetIPAddress();
};