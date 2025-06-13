# Simple WDM Driver Example

This repository contains a minimal Windows Driver Model (WDM) driver project compatible with Visual Studio 2022 and WDK 10.

## Contents

- `SimpleDriver.sln` &mdash; Visual Studio solution file.
- `Driver/` &mdash; Contains the driver project files.
  - `SimpleDriver.vcxproj` &mdash; The project configuration.
  - `Driver.c` &mdash; Driver source implementing `DriverEntry`, `DriverUnload`, and basic IRP handlers.
  - `Driver.h` &mdash; Header defining custom IOCTL codes.
  - `SimpleDriver.inf` &mdash; INF file for installation.
- `UserClient/` &mdash; Simple console application demonstrating how to send mouse input to the driver using the new IOCTL.

## Building

1. Open `SimpleDriver.sln` in Visual Studio 2022 with WDK 10 installed.
2. Choose the desired configuration (`Debug` or `Release`) and build the solution.

## Usage

The driver creates a device named `\\Device\\SimpleDriver` with a symbolic link at `\\DosDevices\\SimpleDriver`.
Two IOCTL codes are available:

1. `IOCTL_MY_OPERATION` &ndash; placeholder operation.
2. `IOCTL_SEND_MOUSE_INPUT` &ndash; accepts a `MOUSE_INPUT` structure from user mode and stores it for later processing.

The `MOUSE_INPUT` structure contains X/Y movement and left/right button states.

### User-mode Example

The `UserClient` project builds a simple console application that opens the
driver interface (`\\.\\SimpleDriver`) and sends a sample `MOUSE_INPUT`
structure via `IOCTL_SEND_MOUSE_INPUT`. Run the executable from an elevated
command prompt after loading the driver with OSR Loader.
