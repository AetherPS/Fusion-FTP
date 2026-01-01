#include "stdafx.h"
#include "BaseClient.h"
#include "PassiveClient.h"

PassiveClient::PassiveClient(SceNetId s, SceNetInAddr localAddr)
{
	ServerSocket = sceNetSocket("", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, SCE_NET_IPPROTO_IP);

	// Set Sending and reciving time out to 2s
	int sock_timeout = 2000000;
	sceNetSetsockopt(ServerSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_SNDTIMEO, &sock_timeout, sizeof(sock_timeout));
	sceNetSetsockopt(ServerSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_RCVTIMEO, &sock_timeout, sizeof(sock_timeout));

	// Make sure every time we can rebind to the port.
	int reusePort = 1;
	sceNetSetsockopt(ServerSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_REUSEPORT, &reusePort, sizeof(reusePort));

	ServerAddr.sin_len = sizeof(ServerAddr);
	ServerAddr.sin_family = SCE_NET_AF_INET;
	ServerAddr.sin_addr.s_addr = SCE_NET_INADDR_ANY;	// Any incoming address.
	ServerAddr.sin_port = sceNetHtons(0);				// Any port is ok.

	auto bindError = sceNetBind(ServerSocket, (SceNetSockaddr*)&ServerAddr, sizeof(ServerAddr));
	if (bindError != 0)
	{
		Logger::Error("Failed to bind Listener\nErrorno: %llX\n", bindError);
		return;
	}

	auto listenError = sceNetListen(ServerSocket, 100);
	if (listenError != 0)
	{
		Logger::Error("Failed to start listening on Socket.\n\nErrorno: %llX\n", listenError);
		return;
	}

	SceNetSocklen_t namelen = sizeof(ServerAddr);
	sceNetGetsockname(ServerSocket, (SceNetSockaddr*)&ServerAddr, &namelen);

	SendDataCRLF(s, va("227 Entering Passive Mode (%hhu,%hhu,%hhu,%hhu,%hhu,%hhu)",
		(localAddr.s_addr >> 0) & 0xFF,
		(localAddr.s_addr >> 8) & 0xFF,
		(localAddr.s_addr >> 16) & 0xFF,
		(localAddr.s_addr >> 24) & 0xFF,
		(ServerAddr.sin_port >> 0) & 0xFF,
		(ServerAddr.sin_port >> 8) & 0xFF).c_str());
}

PassiveClient::~PassiveClient()
{
	sceNetSocketClose(ServerSocket);
}

SceNetId PassiveClient::Connect()
{
	SceNetSockaddrIn ClientAddr = { 0 };
	SceNetSocklen_t addrlen = sizeof(SceNetSockaddrIn);
	return ClientSocket = sceNetAccept(ServerSocket, (SceNetSockaddr*)&ClientAddr, &addrlen);
}