# Pet Tracker Firmware

Firmware for the Pet Tracker device based on nRF9161 DK and nRF Connect SDK / Zephyr.

Current firmware target:

```text
nrf9161dk/nrf9161/ns
```

## Repository Scope

This repository contains:

```text
src/             firmware source code
docs/            firmware, hardware, OTA, and architecture documentation
.github/         firmware CI workflows
CMakeLists.txt   Zephyr application entry point
Kconfig          firmware-specific Kconfig options
prj.conf         Zephyr project configuration
```

Platform/backend/frontend code lives separately:

```text
pet-tracker-platform
```

Firmware/backend integration endpoint documentation lives in the platform repository and may be updated periodically:

```text
D:\projects\petr\petr-platform\docs\firmware\device-firmware-integration.md
```

Check that file when implementing telemetry, commands, configuration, provisioning, update flow, or other backend-facing firmware behavior.

## Current Status

Implemented:

- Minimal Zephyr application skeleton.
- Small `main.c` entry point.
- Explicit device state machine:
  - `BOOT`
  - `IDLE`
  - `ACTIVE`
  - `ALERT`
  - `SLEEP`
  - `ERROR`
- Boot diagnostics with Zephyr logging.
- Default firmware config:
  - device ID
  - firmware version
  - telemetry interval
- Low-frequency heartbeat using `k_sleep()`.
- Manual boot/UART test procedure.

Not implemented yet:

- LTE modem bring-up.
- MQTT connection and telemetry publish.
- GNSS location.
- Command handling.
- BMA400, buzzer, vibration motor drivers.
- Low power policy beyond the basic sleeping heartbeat loop.

## Source Layout

```text
src/
  main.c
  app/
    app_config.c
    app_config.h
    app_state.c
    app_state.h
  platform/
    diagnostics.c
    diagnostics.h
```

Planned modules are documented in:

```text
docs/architecture/firmware-architecture.md
```

## Target Hardware

MVP:

- nRF9161 DK
- SIM card
- GNSS antenna
- BMA400 accelerometer, planned
- Buzzer, planned
- Vibration motor via MOSFET, planned

## SDK

```text
nRF Connect SDK v3.3.0
Zephyr OS
Board target: nrf9161dk/nrf9161/ns
```

## Prerequisites

Install and activate nRF Connect SDK v3.3.0 with the Zephyr tools available in your shell.

Required command:

```powershell
west --version
```

If `west` is not found, open the nRF Connect SDK terminal from the Toolchain Manager or source the SDK environment before building.

On this Windows workstation, the nRF Connect VS Code extension may build successfully while a plain PowerShell cannot find `west`. In that case, run `west` through the bundled toolchain Python:

```powershell
& C:\ncs\toolchains\936afb6332\opt\bin\python.exe -m west --version
```

For convenience in the current PowerShell session:

```powershell
function west { & C:\ncs\toolchains\936afb6332\opt\bin\python.exe -m west @args }
west --version
```

If running Zephyr scripts such as Twister from a plain PowerShell, also put the bundled toolchain binaries on `PATH`:

```powershell
$toolchain = "C:\ncs\toolchains\936afb6332"
$env:ZEPHYR_BASE = "D:\ncs\v3.3.0\zephyr"
$env:ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"
$env:ZEPHYR_SDK_INSTALL_DIR = "$toolchain\opt\zephyr-sdk"
$env:PATH = "$toolchain\opt\bin;$toolchain\opt\bin\Scripts;$env:PATH"
function west { & "$toolchain\opt\bin\python.exe" -m west @args }
```

## Build

From the repository root:

```powershell
west build -b nrf9161dk/nrf9161/ns .
```

Clean rebuild:

```powershell
west build -b nrf9161dk/nrf9161/ns . --pristine
```

Use a clean rebuild after changing `Kconfig`, `prj.conf`, `CMakeLists.txt`, board files, or overlays.

Known non-blocking warnings with nRF Connect SDK v3.3.0:

```text
warning: Deprecated symbol PARTITION_MANAGER_ENABLED is enabled.
warning: Experimental symbol TFM_EXPERIMENTAL is enabled.
```

These warnings come from the Nordic/Zephyr sysbuild configuration for this target, not from application code in `src/`. They are acceptable during MVP bring-up if the build continues and produces firmware output.

## Flash

Connect the nRF9161 DK over USB/J-Link, then run:

```powershell
west flash
```

If multiple boards are connected, select the correct J-Link serial number when prompted by the Nordic tooling.

## UART Logs

After flashing, open the UART log console for the DK and reset the board.

Expected boot logs include:

```text
Pet Tracker firmware boot
Device ID: dev-001
Firmware version: 0.1.0
Telemetry interval: 300 seconds
Device state initialized: BOOT
Device state: BOOT -> IDLE (boot complete)
Heartbeat: state=IDLE
```

The heartbeat interval is configured by:

```text
CONFIG_PET_TRACKER_HEARTBEAT_INTERVAL_SECONDS
```

Default value:

```text
60 seconds
```

## Manual Tests

Manual hardware tests live in:

```text
docs/firmware/manual-tests.md
docs/firmware/testing-strategy.md
```

Current hardware docs:

```text
docs/hardware/hardware-inventory.md
docs/hardware/prototype-wiring.md
docs/firmware/v0-firmware-plan.md
docs/firmware/implementation-roadmap.md
```

Current tests:

```text
TEST-HW-001: Boot and UART Logs
TEST-HW-007: Grove Active Buzzer
TEST-HW-008: Vibration Motor
```

## Automated Tests

Pure firmware logic tests live in:

```text
tests/app_core
```

Run with Twister as a build-only test on the firmware target:

```powershell
$toolchain = "C:\ncs\toolchains\936afb6332"
$env:ZEPHYR_BASE = "D:\ncs\v3.3.0\zephyr"
$env:ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"
$env:ZEPHYR_SDK_INSTALL_DIR = "$toolchain\opt\zephyr-sdk"
$env:PATH = "$toolchain\opt\bin;$toolchain\opt\bin\Scripts;$env:PATH"
& "$toolchain\opt\bin\python.exe" "$env:ZEPHYR_BASE\scripts\twister" -T tests/app_core -p nrf9161dk/nrf9161/ns --build-only -O D:\tw\app_core
```

`native_sim` execution requires a host compiler/toolchain. If `native_sim` is filtered by Twister on Windows, use the `nrf9161dk/nrf9161/ns --build-only` command above until a host test toolchain is configured.

Use a short Twister output path on Windows. The nRF9161 non-secure build includes TF-M, and long paths under the repository `twister-out` directory can break TF-M generated file creation.

Or build the test app directly:

```powershell
west build -b nrf9161dk/nrf9161/ns tests/app_core -d build/app_core_tests --pristine
```

Current coverage:

```text
app_state valid and invalid transitions
app_state names
app_config defaults
```

## Development Milestones

```text
1. firmware skeleton - in progress
2. boot logging - in progress
3. state machine - in progress
4. modem/LTE
5. MQTT
6. GNSS
7. telemetry publish
8. command subscribe
9. BMA400 integration
10. buzzer and vibration drivers
11. low power mode
12. OTA planning
```
