#pragma once

class ActiveClient : public BaseClient
{
public:
	ActiveClient(SceNetId s, std::string portArgs);
	~ActiveClient();

	virtual SceNetId Connect() override;

private:
	SceNetSockaddrIn ClientAddr;
};
