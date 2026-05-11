# Firmware Implementation Roadmap

This roadmap describes the current firmware state and the planned implementation sequence. Keep it updated as milestones are completed or the platform contract changes.

## Current Implementation State

Implemented firmware code:

- `main.c`: boot banner, state initialization, `BOOT -> IDLE`, optional buzzer startup tone, and heartbeat loop.
- `app/app_state.c`: explicit device state machine with `BOOT`, `IDLE`, `ACTIVE`, `ALERT`, `SLEEP`, and `ERROR`.
- `app/app_config.c`: static defaults for `device_id`, firmware version, and telemetry interval.
- `platform/diagnostics.c`: boot diagnostics using Zephyr logging.
- `drivers/buzzer.c`: GPIO driver for the Grove active buzzer with on/off, beep, and startup tone helpers.
- `app.overlay`: `buzzer0` alias mapped to `D2 / P0.16`.

Implemented configuration:

- Zephyr logging enabled.
- Firmware version configured as `0.1.0`.
- Heartbeat interval configured as 60 seconds.
- Buzzer driver enabled.
- One-shot buzzer startup tone available but disabled outside manual buzzer bring-up.

Implemented documentation:

- Firmware architecture notes.
- Hardware inventory.
- Prototype V0 wiring.
- Manual hardware tests.
- Testing strategy.
- MQTT contract draft.
- OTA strategy notes.

Not implemented yet:

- BMA400 driver or I2C detection test.
- LTE modem service.
- MQTT service.
- Telemetry payload builder.
- Command parser and dispatcher.
- Device config subscription and storage.
- GNSS/location service.
- Persistent storage.
- Offline telemetry queue.
- Power policy.
- OTA/update logic.
- Automated unit tests.

## Important Contract Note

Firmware/backend endpoint and protocol details live in the platform repository:

```text
D:\projects\petr\petr-platform\docs\firmware\device-firmware-integration.md
```

Check that file before implementing telemetry, commands, configuration, provisioning, update flow, or other backend-facing behavior.

The platform integration guide currently recommends compact MQTT topics for normal firmware operation:

```text
Telemetry publish: pt/1/d/{deviceId}/t
Command subscribe: pt/1/d/{deviceId}/c
Command ack:       pt/1/d/{deviceId}/c/{commandId}/a
Config subscribe:  pt/1/d/{deviceId}/cfg
```

Before implementing MQTT, align `docs/firmware/mqtt-contract.md` with the latest platform integration guide.

## Phase 0: V0 Hardware Bring-Up

### Step 0.1: Verify Current Buzzer Build

Goal:

- Prove the connected Grove active buzzer on `D2 / P0.16` works with firmware.

Tasks:

- Build for `nrf9161dk/nrf9161/ns`.
- Flash the nRF9161 DK.
- Confirm one short startup tone.
- Confirm UART logs include buzzer initialization and startup tone completion.
- Record result in `TEST-HW-007`.

Expected command:

```powershell
west build -b nrf9161dk/nrf9161/ns . --pristine
west flash
```

### Step 0.2: Add Vibration Motor Driver

Goal:

- Prove `D3 / P0.17` can safely control the vibration motor through the IRLZ44N MOSFET.

Tasks:

- Extend `app.overlay` with `vibration0 -> gpio0 pin 17`.
- Add `drivers/vibration_motor.c`.
- Add `drivers/vibration_motor.h`.
- Add Kconfig flags for vibration support.
- Cap vibration pulse duration in firmware.
- Add a short boot/manual test pulse only when explicitly enabled.
- Update `TEST-HW-008`.

Safety default:

```text
CONFIG_PET_TRACKER_VIBRATION_MOTOR=y
# CONFIG_PET_TRACKER_VIBRATION_BOOT_TEST=y
```

Hardware guardrails:

- Motor must never be connected directly to MCU GPIO.
- Use 220 ohm gate resistor.
- Use 10k gate pull-down.
- Use 1N5819 flyback diode across the motor, cathode toward motor positive.
- Keep first pulses short, for example 200 ms.

### Step 0.3: Add Initial Automated Tests

Goal:

- Start the automated test foundation before protocol work grows.

Status:

- Initial `tests/app_core` Zephyr `ztest` app added.
- Covers app state transitions, invalid states, state names, and app config defaults.
- Build-only verification passed on `nrf9161dk/nrf9161/ns` using Twister with short output path `D:\tw\app_core`.
- Native execution with `native_sim` is pending host toolchain setup.

Tasks:

- Add Zephyr `ztest` or a practical host-test setup.
- Test `app_state` valid transitions, invalid transitions, and state names.
- Test `app_config` defaults.
- Make buzzer startup tone timing testable where practical.
- Document test execution in README.

### Step 0.4: Add BMA400 I2C Detection

Goal:

- Prove the Grove BMA400 is reachable on Arduino I2C.

Tasks:

- Add a devicetree node for BMA400 on Arduino I2C if Zephyr driver support is available.
- Add minimal `drivers/bma400_sensor.c/h` or a detection service.
- Log device readiness or chip ID if supported.
- Add manual BMA400 I2C detection test.
- Keep BMA400 INT unassigned until wake-on-motion work.

### Step 0.5: Reduce `main.c`

Goal:

- Keep `main.c` small before modem and protocol services are added.

Tasks:

- Move hardware bring-up orchestration into an app/service layer.
- Keep `main.c` responsible only for diagnostics, app initialization, and app run.
- Preserve clear boot logs.

## Phase 1: Backend Protocol And Telemetry

### Step 1.1: Align MQTT Contract

Goal:

- Make firmware docs match the platform integration guide.

Tasks:

- Update `docs/firmware/mqtt-contract.md` to prefer compact topics.
- Keep readable topics as debug fallback.
- Document command ACK statuses and config topic behavior.
- Add examples for compact telemetry and command ACK.

### Step 1.2: Add Telemetry Payload Builder

Goal:

- Build compact telemetry payloads without requiring LTE or GNSS yet.

Tasks:

- Add `services/telemetry_service.c`.
- Add a small telemetry model with protocol version, timestamp, connectivity, activity, battery, and optional location.
- Use fixed-point location fields only when a valid fix exists.
- Include firmware/device state where the selected payload format requires it.
- Add unit tests for required fields, optional fields, and invalid combinations.

Initial compact topic:

```text
pt/1/d/{deviceId}/t
```

### Step 1.3: Add Command Parser And Dispatcher

Goal:

- Parse backend commands and map them to firmware actions.

Tasks:

- Add `services/command_service.c`.
- Support `BUZZ` first.
- Validate `commandId`, `expiresAt`, and payload.
- Ignore expired commands but still ACK them.
- Do not execute the same `commandId` twice.
- Add ACK payload builder with `OK`, `EXPIRED`, `UNSUPPORTED`, and `FAILED`.
- Add unit tests for valid, invalid, expired, duplicate, and unsupported commands.

Initial ACK topic:

```text
pt/1/d/{deviceId}/c/{commandId}/a
```

### Step 1.4: Add Local MQTT Service

Goal:

- Send telemetry and receive commands through a local EMQX broker before LTE.

Tasks:

- Add `services/mqtt_service.c`.
- Add topic construction helpers.
- Add local broker configuration.
- Publish compact telemetry.
- Subscribe to compact command topic.
- Publish command ACK.
- Add integration test checklist with local EMQX.

Initial local broker:

```text
host: localhost
port: 1883
TLS: false
```

## Phase 2: LTE, GNSS, And Real Device Telemetry

### Step 2.1: Add LTE Modem Service

Goal:

- Register on LTE-M/NB-IoT and expose connection state to the app.

Tasks:

- Add `services/modem_service.c`.
- Initialize modem library.
- Register to network with timeout.
- Use exponential backoff.
- Log useful modem status without noisy loops.
- Add manual LTE registration test.

### Step 2.2: Run MQTT Over LTE

Goal:

- Publish telemetry and receive commands through the public broker from the DK.

Tasks:

- Configure firmware with public MQTT host from deployment.
- Provision the device and store/copy MQTT credentials for the test.
- Connect over LTE.
- Publish compact telemetry.
- Receive `BUZZ` command.
- Publish ACK.
- Add manual test evidence.

Production direction:

- TLS on port 8883.
- Avoid hardcoded secrets in source.
- Keep reconnect attempts power-aware.

### Step 2.3: Add GNSS Location Service

Goal:

- Acquire location fixes and include them in telemetry.

Tasks:

- Add `services/location_service.c`.
- Request GNSS fix with timeout.
- Track stale fix vs valid fix.
- Convert latitude/longitude to fixed-point integer format for compact telemetry.
- Add fake-provider tests for timeout/stale/valid fix logic.
- Add outdoor manual GNSS test.

### Step 2.4: Add Device Config Handling

Goal:

- Apply retained backend config messages.

Tasks:

- Subscribe to compact config topic.
- Parse config version `cfg`, reporting profile `rp`, intervals `ri`, and batch size.
- Keep highest applied config version.
- Ignore older config messages.
- Apply telemetry interval based on reporting profile.
- Add unit tests for version handling and profile interval selection.

Config topic:

```text
pt/1/d/{deviceId}/cfg
```

## Phase 3: Persistence, Offline Behavior, And Power

### Step 3.1: Add Storage Service

Goal:

- Persist credentials/config and support basic offline behavior.

Tasks:

- Add `services/storage_service.c`.
- Store device ID and MQTT credentials after provisioning flow exists.
- Store highest applied config version.
- Store small dedupe cache for command IDs if practical.
- Add tests for config persistence and error handling.

### Step 3.2: Add Offline Telemetry Queue

Goal:

- Avoid silently dropping telemetry when network is unavailable.

Tasks:

- Define queue capacity and drop policy.
- Queue telemetry points while offline.
- Batch telemetry when reconnecting.
- Preserve original timestamps.
- Add unit tests for enqueue, dequeue, capacity, and drop policy.

### Step 3.3: Add Power Policy

Goal:

- Make firmware battery-aware before wearable tests.

Tasks:

- Add `services/power_service.c`.
- Decide when LTE can disconnect.
- Gate sleep on pending telemetry and commands.
- Use adaptive intervals based on movement, battery, and profile.
- Avoid LTE reconnect storms.
- Add retry/backoff tests.
- Add current draw manual tests.

## Phase 4: OTA And Production Readiness

### Step 4.1: Add Update State Logic

Goal:

- Model update behavior before implementing real LTE FOTA.

Tasks:

- Add update state enum and transitions.
- Validate battery/config preconditions.
- Emit update state telemetry/events.
- Add tests for success, failure, rejected, and rollback-needed paths.

### Step 4.2: Plan MCUBoot/Rollback

Goal:

- Prepare safe OTA foundations.

Tasks:

- Document bootloader choice and partition requirements.
- Require firmware validation.
- Require rollback support before production OTA.
- Add manual version A -> version B flashing test.

### Step 4.3: Future LTE FOTA

Goal:

- Add LTE FOTA only after MQTT, storage, power, and update state logic are stable.

Tasks:

- Use public server endpoint.
- Use TLS.
- Check battery before update.
- Report update progress and final state.
- Keep FOTA tests separate from normal CI.

## Ongoing Definition Of Done

Every firmware task should finish with:

- Code builds for `nrf9161dk/nrf9161/ns`, if the SDK shell is available.
- Automated tests are added for pure logic where practical.
- Manual hardware tests are added or updated for hardware behavior.
- Relevant docs are updated when contracts, pins, payloads, or behavior change.
- Final notes report what was tested and what was not tested.
