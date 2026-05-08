# Firmware Architecture

## Overview

The firmware should be built as a small modular Zephyr application.

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

## Device States

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

## Main Loop Concept

```text
boot
↓
load config
↓
init modem
↓
connect LTE
↓
connect MQTT
↓
start telemetry loop
↓
process commands
↓
sleep/adaptive tracking
```

## Thread Model

MVP can start simple with work queues and timers.

Future structure:

```text
main thread
mqtt thread
telemetry work queue
sensor work queue
command handler
```

## Logging

Use Zephyr logging:

```c
LOG_INF("message");
LOG_ERR("error");
LOG_DBG("debug");
```

## Configuration

Config should be received from backend and cached locally.

Examples:

- telemetry interval
- GNSS interval
- motion threshold
- alert patterns
- power profile
