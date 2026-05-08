# OTA Strategy

## Current State

During development, firmware is flashed through USB/J-Link using the onboard debugger on nRF9161 DK.

## Important Constraint

nRF9161 does not have BLE.

Therefore BLE DFU requires a companion BLE chip.

## Planned BLE Companion

Recommended chip:

```text
nRF52840
```

Architecture:

```text
nRF52840
 ├─ BLE
 ├─ DFU
 └─ sensors

↓ UART/SPI

nRF9161
 ├─ LTE
 └─ GNSS
```

## OTA Roadmap

```text
1. USB/J-Link flashing
2. Stable firmware
3. MQTT/LTE communication
4. Power optimization
5. Add nRF52840 BLE companion
6. BLE DFU from mobile app
7. LTE FOTA
```

## BLE DFU Benefits

- Lower battery usage than LTE FOTA
- No SIM traffic cost
- User is physically near device
- Easier recovery
- Good mobile app UX

## LTE FOTA Later

LTE FOTA should be added only after the firmware is stable.

Requirements:

- Secure firmware validation
- Rollback support
- Resumable download
- Battery-aware update policy
- Update state reporting
