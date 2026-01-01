#include "stdafx.h"

int main(int argc, char** arg)
{
	// Set up the Logger.
	Logger::Init(true, Logger::LoggingLevels::LogLevelAll);

	// Jailbreak our current process.
	if (!Jailbreak())
	{
		Notify("Failed to jailbreak Process...");
		return 0;
	}

	// Load internal system modules.
	if (!LoadModules())
	{
		Notify("Failed to Load Modules...");
		return 0;
	}

	// Set the Name of this process so it shows up as something other than eboot.bin.
	sceKernelSetProcessName("Fusion FTP Daemon");

	// Start up the thread pool.
	ThreadPool::Init(10);

	// Mount system as R/W
	RemountReadWrite("/dev/da0x4.crypt", "/system");
	RemountReadWrite("/dev/da0x5.crypt", "/system_ex");

	// Mount the procfs.
	MountProcFs();

	// Start up the main FTP.
	MainListener::Init();

	while (true) { sceKernelSleep(1); }

	return 0;
}