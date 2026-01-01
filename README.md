# Fusion-FTP

FTP daemon for the Fusion homebrew enabler on PlayStation 4.

## Features

- Full FTP server implementation for PS4
- Active and passive mode support
- System partition remounting (R/W access to `/system` and `/system_ex`)
- Multi-threaded connection handling
- Process filesystem mounting

## Prerequisites

Before building this project, ensure you have the following installed:

- **Official PlayStation 4 SDK** - Required for PS4 development
- **Visual Studio** - For building the project and dependencies
- **Python 3** - Required for build scripts

## Cloning

This project uses git submodules for its dependencies. Clone with:

```bash
git clone --recurse-submodules https://github.com/AetherPS/Fusion-FTP
```

If you've already cloned without submodules, initialize them with:

```bash
git submodule update --init --recursive
```

## Building

### 1. Build Dependencies

The project has several dependencies located in the `External` folder that must be built first:

#### StubMaker
This provides stub libraries for internal PS4 system modules.

#### libFusionDriver
Navigate to `External\libFusionDriver` and open `libFusionDriver.sln` in Visual Studio. Build the solution.

#### libUtils
Navigate to `External\libUtils` and open `libUtils.sln` in Visual Studio. Build the solution.

### 2. Build Fusion-FTP

1. Open `Fusion-FTP.slnx` in Visual Studio
2. Select your desired configuration (Debug/Release)
3. Build the solution (Build > Build Solution or press F7)

## Project Structure

```
Fusion-FTP/
├── Fusion-FTP/          # Main source code
│   ├── main.cpp         # Entry point and initialization
│   ├── MainListener.*   # Main FTP server listener
│   ├── FTPClient.*      # FTP client connection handler
│   ├── ActiveClient.*   # Active mode FTP implementation
│   ├── PassiveClient.*  # Passive mode FTP implementation
│   ├── FileClient.*     # File transfer handler
│   ├── DirectoryProvider.* # Directory listing provider
│   └── Misc.*           # Utility functions
├── External/            # Dependencies (git submodules)
│   ├── StubMaker/       # PS4 system module stubs
│   ├── libFusionDriver/ # Fusion driver library
│   └── libUtils/        # Utility library
└── Scripts/             # Build scripts
    └── make_fself.py    # FSELF creation script
```

## Notes

- This project requires a PlayStation 4 with the Fusion or GoldHEN installed
- The FTP daemon runs with elevated privileges and provides read/write access to system partitions

## Contributing

Contributions are welcome. Please ensure all code follows the existing style and structure.
