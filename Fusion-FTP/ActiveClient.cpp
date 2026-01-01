#include "stdafx.h"
#include "BaseClient.h"
#include "ActiveClient.h"

ActiveClient::ActiveClient(SceNetId s, std::string portArgs)
{
	// Initialize ClientAddr to zero (safe default state)
	memset(&ClientAddr, 0, sizeof(ClientAddr));

	// Parse PORT command arguments: h1,h2,h3,h4,p1,p2
	// where IP = h1.h2.h3.h4 and port = p1*256 + p2
	int h1, h2, h3, h4, p1, p2;
	if (sscanf(portArgs.c_str(), "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2) != 6)
	{
		Logger::Error("Failed to parse PORT command arguments: %s", portArgs.c_str());
		SendDataCRLF(s, "501 Syntax error in parameters or arguments.");
		return;
	}

	// Validate IP address octets
	if (h1 < 0 || h1 > 255 || h2 < 0 || h2 > 255 ||
		h3 < 0 || h3 > 255 || h4 < 0 || h4 > 255 ||
		p1 < 0 || p1 > 255 || p2 < 0 || p2 > 255)
	{
		Logger::Error("Invalid PORT command values: %s", portArgs.c_str());
		SendDataCRLF(s, "501 Syntax error in parameters or arguments.");
		return;
	}

	// Build the client address structure
	ClientAddr.sin_len = sizeof(ClientAddr);
	ClientAddr.sin_family = SCE_NET_AF_INET;
	ClientAddr.sin_addr.s_addr = (h1 << 0) | (h2 << 8) | (h3 << 16) | (h4 << 24);
	ClientAddr.sin_port = sceNetHtons((p1 << 8) | p2);

	Logger::Info("PORT: Client requesting connection to %d.%d.%d.%d:%d",
		h1, h2, h3, h4, (p1 << 8) | p2);

	SendDataCRLF(s, "200 PORT command successful.");
}

ActiveClient::~ActiveClient()
{
	// Base class destructor will handle socket cleanup
}

SceNetId ActiveClient::Connect()
{
	// Check if PORT command was successfully parsed
	if (ClientAddr.sin_port == 0)
	{
		Logger::Error("Cannot connect: PORT command was not successfully parsed");
		return -1;
	}

	// Create a socket to connect to the client
	ClientSocket = sceNetSocket("", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, SCE_NET_IPPROTO_IP);
	if (ClientSocket < 0)
	{
		Logger::Error("Failed to create socket for active connection: 0x%08X", ClientSocket);
		return -1;
	}

	// Set socket timeouts
	int sock_timeout = 2000000; // 2 seconds
	sceNetSetsockopt(ClientSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_SNDTIMEO, &sock_timeout, sizeof(sock_timeout));
	sceNetSetsockopt(ClientSocket, SCE_NET_SOL_SOCKET, SCE_NET_SO_RCVTIMEO, &sock_timeout, sizeof(sock_timeout));

	// Connect to the client
	int connectResult = sceNetConnect(ClientSocket, (SceNetSockaddr*)&ClientAddr, sizeof(ClientAddr));
	if (connectResult < 0)
	{
		Logger::Error("Failed to connect to client: 0x%08X", connectResult);
		sceNetSocketClose(ClientSocket);
		ClientSocket = -1;
		return -1;
	}

	Logger::Info("Successfully connected to client in active mode");
	return ClientSocket;
}