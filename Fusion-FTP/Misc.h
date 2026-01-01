#pragma once

bool LoadModules();
std::vector<uint8_t> RecieveData(SceNetId s, size_t maximumLength);
void SendData(SceNetId s, uint8_t* data, size_t len);
void SendDataCRLF(SceNetId s, std::string data);
std::string removeCRLF(const std::string& input);
std::vector<std::string> TokenizeArgs(const std::string& input);
std::string PermissionFromStat(const SceKernelStat& fileStat);