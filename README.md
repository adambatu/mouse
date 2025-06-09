# Simple WDM Driver Example

This repository contains a minimal Windows Driver Model (WDM) driver project compatible with Visual Studio 2022 and WDK 10.

## Contents

- `SimpleDriver.sln` &mdash; Visual Studio solution file.
- `Driver/` &mdash; Contains the driver project files.
  - `SimpleDriver.vcxproj` &mdash; The project configuration.
  - `Driver.c` &mdash; Driver source implementing `DriverEntry`, `DriverUnload`, and basic IRP handlers.
  - `Driver.h` &mdash; Header defining custom IOCTL codes.
  - `SimpleDriver.inf` &mdash; INF file for installation.

## Building

1. Open `SimpleDriver.sln` in Visual Studio 2022 with WDK 10 installed.
2. Choose the desired configuration (`Debug` or `Release`) and build the solution.

## Usage

The driver creates a device named `\\Device\\SimpleDriver` with a symbolic link at `\\DosDevices\\SimpleDriver`. A custom IOCTL (`IOCTL_MY_OPERATION`) is handled in `DriverDeviceControl`.
