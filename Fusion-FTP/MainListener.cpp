#include "stdafx.h"
#include "MainListener.h"

std::unique_ptr<SocketListener> MainListener::Listener;
std::vector<std::unique_ptr<FTPClient>> MainListener::Clients;
int MainListener::FtpPort = 1337;
char MainListener::IPAddress[16];
SceNetInAddr MainListener::LocalAddr;

void MainListener::ListenerCallback(void* tdParam, SceNetId s, SceNetInAddr sin_addr)
{
	Logger::Info("New Client Connection: %i.%i.%i.%i\n",
		sin_addr.s_addr & 0xFF,
		(sin_addr.s_addr >> 8) & 0xFF,
		(sin_addr.s_addr >> 16) & 0xFF,
		(sin_addr.s_addr >> 24) & 0xFF);

	Clients.push_back(std::make_unique<FTPClient>(s, sin_addr, LocalAddr));
}

void MainListener::SetIPAddress()
{
	SceNetCtlInfo info;
	sceNetCtlGetInfo(SCE_NET_CTL_INFO_IP_ADDRESS, &info);

	strcpy(IPAddress, info.ip_address);
	sceNetInetPton(SCE_NET_AF_INET, IPAddress, &LocalAddr);
}

void MainListener::Init()
{
	Listener = std::make_unique<SocketListener>(ListenerCallback,
		[](void*)
	{
		NotifyCustom("", "Network connection lost Orbis FTP is shutting down...");
	},
		[](void*)
	{
		SetIPAddress();
		NotifyCustom("", "Orbis FTP Re-Started: %s:%d", IPAddress, FtpPort);
	},
		nullptr, FtpPort, 0, SCE_NET_IPPROTO_IP);

	SetIPAddress();

	Logger::Info("Listener Started %s:%d\n", IPAddress, FtpPort);
	NotifyCustom("", "Orbis FTP Running: %s:%d", IPAddress, FtpPort);
}

void MainListener::Term()
{
	Clients.clear();
}