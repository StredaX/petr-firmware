# AGENTS.md — Pet Tracker Firmware

This file is the main instruction file for AI coding agents working in the firmware repository.

The goal is to keep firmware work focused, safe, power-aware, and aligned with the Pet Tracker platform contract.

---

## 1. Repository Purpose

This repository contains firmware for the Pet Tracker device.

Target hardware:

```text
nRF9161 DK
nRF Connect SDK / Zephyr
Board target: nrf9161dk/nrf9161/ns
```

Planned hardware:

```text
BMA400 accelerometer
buzzer
vibration motor via MOSFET
battery temperature sensor
future nRF52840 BLE companion
```

The cloud/backend/frontend platform lives in:

```text
pet-tracker-platform
```

Firmware/backend integration endpoints are documented in the platform repository and may be updated periodically:

```text
D:\projects\petr\petr-platform\docs\firmware\device-firmware-integration.md
```

Use that file when implementing or changing firmware behavior that talks to backend endpoints, device registration, telemetry ingestion, commands, configuration, update flow, or any API-level contract.

---

## 2. Documentation Map

Read only the smallest relevant documents.

### Firmware architecture

```text
docs/architecture/firmware-architecture.md
```

Use when:
- adding services
- changing state machine
- changing module structure
- changing threading/work queues

### Hardware plan

```text
docs/hardware/hardware-plan.md
```

Use when:
- adding GPIO
- wiring buzzer/motor
- integrating BMA400
- changing pin assignments

### MQTT contract

```text
docs/firmware/mqtt-contract.md
```

Use when:
- publishing telemetry
- subscribing to commands
- changing payloads
- changing topic names

### Platform firmware integration endpoints

```text
D:\projects\petr\petr-platform\docs\firmware\device-firmware-integration.md
```

Use when:
- implementing backend-facing firmware behavior
- changing telemetry ingestion or command handling assumptions
- adding device registration/provisioning behavior
- touching firmware update endpoint flow
- checking the latest platform-side endpoint contract

### OTA strategy

```text
docs/ota/ota-strategy.md
```

Use when:
- touching firmware update logic
- adding bootloader-related behavior
- planning BLE DFU
- planning LTE FOTA

### Product specification

```text
docs/product-specification.md
```

Use when:
- changing product-level behavior
- changing telemetry meaning
- changing alert behavior
- changing power strategy

---

## 3. Token-Saving Rules

### Do

- Read only relevant docs and source files.
- Prefer targeted changes.
- Keep firmware patches small.
- Explain hardware assumptions briefly.
- Reuse existing Zephyr APIs and Nordic samples.
- Prefer simple reliable firmware over clever abstractions.
- Keep logs useful but not noisy.

### Do Not

- Do not scan the whole repository unless necessary.
- Do not rewrite unrelated modules.
- Do not introduce large frameworks.
- Do not paste long unchanged code in responses.
- Do not change MQTT payloads without updating docs.
- Do not add blocking sleeps in production logic unless justified.
- Do not add battery-expensive loops.
- Do not assume Wi-Fi or BLE exists on nRF9161.

Important hardware constraint:

```text
nRF9161 has LTE-M / NB-IoT / GNSS, but no Wi-Fi and no BLE.
```

BLE DFU requires a future companion chip such as nRF52840.

---

## 4. Firmware Architecture Rules

Preferred structure:

```text
src/
 ├─ main.c
 ├─ app/
 │   ├─ app_state.c
 │   ├─ app_config.c
 │   └─ app_events.c
 ├─ services/
 │   ├─ modem_service.c
 │   ├─ mqtt_service.c
 │   ├─ telemetry_service.c
 │   ├─ location_service.c
 │   ├─ command_service.c
 │   ├─ power_service.c
 │   └─ storage_service.c
 ├─ drivers/
 │   ├─ buzzer.c
 │   ├─ vibration_motor.c
 │   └─ bma400_sensor.c
 └─ platform/
     ├─ board_pins.c
     └─ diagnostics.c
```

Rules:

- `main.c` should stay small.
- Hardware-specific code belongs in `drivers/` or `platform/`.
- Cloud protocol logic belongs in `mqtt_service`.
- Product telemetry composition belongs in `telemetry_service`.
- Command parsing/dispatch belongs in `command_service`.
- Power decisions belong in `power_service`.
- Do not mix GPIO logic with business state machine.
- Keep modules testable where practical.

---

## 5. Device State Machine

Use explicit states.

```c
typedef enum {
    DEVICE_STATE_BOOT,
    DEVICE_STATE_IDLE,
    DEVICE_STATE_ACTIVE,
    DEVICE_STATE_ALERT,
    DEVICE_STATE_SLEEP,
    DEVICE_STATE_ERROR
} device_state_t;
```

Rules:

- State transitions should be logged.
- State transitions should be deterministic.
- Avoid hidden state changes in low-level drivers.
- Error states should expose reason codes.
- Sleep transitions must consider pending MQTT/telemetry work.

---

## 6. MQTT Rules

Broker:

```text
EMQX
MQTT over TLS
Port: 8883
```

Client ID:

```text
device-{deviceId}
```

Publish topics:

```text
pt/v1/devices/{deviceId}/telemetry
pt/v1/devices/{deviceId}/state
pt/v1/devices/{deviceId}/events
pt/v1/devices/{deviceId}/commands/{commandId}/ack
```

Subscribe topics:

```text
pt/v1/devices/{deviceId}/commands
pt/v1/devices/{deviceId}/config
```

Rules:

- Use QoS intentionally.
- Telemetry can usually use QoS 0 or 1 depending on reliability needs.
- Commands should use acknowledgement.
- Include firmware version in telemetry.
- Include device state in telemetry.
- Include timestamp if available.
- Do not change topics without updating docs and backend contract.

---

## 7. Power Management Rules

Battery life is a core product requirement.

Do:

- Prefer event-driven logic.
- Use wake-on-motion with BMA400 when available.
- Use adaptive telemetry intervals.
- Sleep whenever possible.
- Batch telemetry when offline.
- Avoid frequent GNSS fixes unless needed.
- Avoid LTE reconnect storms.
- Use exponential backoff.

Do not:

- Poll sensors aggressively without reason.
- Keep LTE active unnecessarily.
- Keep GNSS active continuously in normal mode.
- Log too much in production mode.
- Use busy loops.

Power states:

```text
BOOT
IDLE
ACTIVE
ALERT
SLEEP
ERROR
```

---

## 8. Hardware Rules

### nRF9161 DK

Use board target:

```text
nrf9161dk/nrf9161/ns
```

### BMA400

Planned connection:

```text
VCC  → 3.3V
GND  → GND
SDA  → I2C SDA
SCL  → I2C SCL
INT  → GPIO interrupt
```

Use BMA400 for:

- activity detection
- inactivity detection
- wake-on-motion
- sleep/activity classification

### Buzzer

```text
GPIO → buzzer
```

### Vibration Motor

```text
GPIO → MOSFET → motor
```

Never drive a motor directly from MCU GPIO.

---

## 9. Firmware Testing Rules

Firmware tests should focus on logic that can be tested without hardware.

### Unit-testable Logic

Good candidates:

- state transitions
- telemetry payload creation
- command parsing
- config validation
- retry/backoff calculation
- activity classification thresholds
- risk/event mapping
- offline queue behavior

Avoid unit-testing:

- Zephyr internals
- Nordic modem internals
- raw GPIO behavior
- vendor driver implementation

### Hardware Tests

Hardware tests should be documented as manual test procedures.

Examples:

```text
TEST-HW-001: Boot and UART logs
TEST-HW-002: LED/button GPIO
TEST-HW-003: LTE registration
TEST-HW-004: MQTT connection
TEST-HW-005: GNSS fix
TEST-HW-006: BMA400 interrupt
TEST-HW-007: buzzer pattern
TEST-HW-008: vibration pattern
```

### Test Documentation

For manual tests, record:

```text
hardware
firmware version
SDK version
board target
steps
expected result
actual result
notes
```

### CI Tests

CI should run:

- build checks
- formatting checks if configured
- unit tests for pure logic
- static analysis if configured

CI should not require:

- physical device
- SIM card
- LTE network
- GNSS signal
- EMQX broker unless explicitly running integration tests

---

## 10. Logging Rules

Use Zephyr logging.

Preferred style:

```c
LOG_INF("LTE connected");
LOG_WRN("GNSS fix timeout");
LOG_ERR("MQTT connection failed: %d", err);
```

Rules:

- Logs should help diagnose field issues.
- Avoid logging secrets.
- Avoid excessive logs in loops.
- Use module-specific log names.
- Keep boot logs clear.

---

## 11. Error Handling Rules

Firmware must be resilient.

Do:

- Retry LTE/MQTT with exponential backoff.
- Cache telemetry while offline.
- Acknowledge commands with success/failure.
- Report error reason in state/event telemetry.
- Keep device usable after transient failures.

Do not:

- reboot endlessly without backoff.
- drop telemetry silently.
- block forever waiting for network.
- assume GNSS fix is always available.

---

## 12. OTA Rules

Current MVP:

```text
USB/J-Link flashing
```

Future:

```text
BLE DFU via nRF52840 companion
LTE FOTA later
```

Rules:

- Do not implement LTE FOTA prematurely.
- OTA must support rollback before production.
- Firmware validation is required.
- Battery level must be checked before update.
- Update state must be reported to backend.

---

## 13. Definition of Done

A firmware task is done when:

- code builds for `nrf9161dk/nrf9161/ns`
- relevant docs are updated if contracts changed
- tests are added for pure logic where practical
- hardware assumptions are documented
- no unrelated modules are changed
- response includes concise summary and test result

---

## 14. Preferred Agent Workflow

For non-trivial tasks:

```text
1. Read relevant docs only.
2. Inspect related source files.
3. Identify minimal firmware change.
4. Implement.
5. Add/update unit tests for pure logic.
6. Build target if possible.
7. Summarize changed files and test results.
```

For hardware tasks:

```text
1. Check hardware docs.
2. Check board/device tree assumptions.
3. Implement minimal driver/service integration.
4. Add manual test procedure.
5. Build.
6. Report exact test steps.
```
