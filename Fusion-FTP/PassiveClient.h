#pragma once

class PassiveClient : public BaseClient
{
public:
	PassiveClient(SceNetId s, SceNetInAddr localAddr);
	~PassiveClient();

	virtual SceNetId Connect() override;

private:
	bool IsRunning;
	SceNetId ServerSocket;
	SceNetSockaddrIn ServerAddr;
};
