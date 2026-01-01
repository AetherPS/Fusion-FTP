# Fusion-FTP

FTP daemon for the Fusion homebrew enabler on PlayStation 4.

## Features

- Full FTP server implementation for PS4
- Passive mode support
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

The build process will:
- Compile the FTP daemon
- Link against the PS4 SDK libraries
- Execute post-build scripts using Python to create the final FSELF executable

## Output

The compiled output will be in the appropriate build directory (Debug/Release) with the final FSELF ready for deployment to a PS4 running the Fusion homebrew enabler.

## Usage

Deploy the built FSELF to your PS4 and launch it through the Fusion homebrew enabler. The FTP server will:
- Start listening on the standard FTP port (21)
- Provide access to the PS4 filesystem
- Remount system partitions as read/write for extended access

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

- This project requires a PlayStation 4 with the Fusion homebrew enabler installed
- The FTP daemon runs with elevated privileges and provides read/write access to system partitions
- Ensure you have proper authorization before using this software on any PS4 system

## Contributing

Contributions are welcome. Please ensure all code follows the existing style and structure.
