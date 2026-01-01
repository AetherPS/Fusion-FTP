#pragma once

class BaseClient
{
public:
	SceNetId ClientSocket = -1;

	virtual inline ~BaseClient()
	{
		Disconnect();
	}

	virtual SceNetId Connect()
	{
		return ClientSocket;
	};

	void Disconnect()
	{
		if (ClientSocket != -1)
			sceNetSocketClose(ClientSocket);
	}

	void SendMessage(std::string msg)
	{
		if (ClientSocket == -1)
		{
			Logger::Error("SendMessage when socket not connected.\n");
			return;
		}

		SendDataCRLF(ClientSocket, msg);
	}

	bool SendData(uint8_t* data, size_t length)
	{
		if (ClientSocket == -1)
		{
			Logger::Error("SendMessage when socket not connected.\n");
			return false;
		}

		return sceNetSend(ClientSocket, data, length, 0) > 0;
	}

	int RecieveData(uint8_t* data, size_t length)
	{
		if (ClientSocket == -1)
		{
			Logger::Error("SendMessage when socket not connected.\n");
			return -1;
		}

		return sceNetRecv(ClientSocket, data, length, 0);
	}

private:

};
