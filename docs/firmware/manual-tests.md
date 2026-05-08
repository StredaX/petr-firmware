# Firmware Manual Tests

Manual hardware tests should be recorded when firmware is run on the nRF9161 DK.

Overall testing policy lives in:

```text
docs/firmware/testing-strategy.md
```

## TEST-HW-001: Boot and UART Logs

Hardware:

- nRF9161 DK
- USB/J-Link connection
- Board target: `nrf9161dk/nrf9161/ns`

Firmware version:

- `0.1.0`

SDK version:

- nRF Connect SDK v3.3.0

Steps:

1. Build the firmware for `nrf9161dk/nrf9161/ns`.
2. Flash the firmware to the nRF9161 DK.
3. Open the UART log console.
4. Reset the board.

Expected result:

- Boot log includes the Pet Tracker banner.
- Device ID is logged.
- Firmware version is logged.
- State transition `BOOT -> IDLE` is logged.
- Heartbeat log appears at the configured interval.

Actual result:

- Not run yet.

Notes:

- The heartbeat uses `k_sleep()` in the MVP main loop to yield to the Zephyr scheduler between logs. This is a temporary bring-up signal, not a sensor polling loop.

## TEST-HW-007: Grove Active Buzzer

Hardware:

- nRF9161 DK
- Grove active buzzer wired per `docs/hardware/prototype-wiring.md`
- USB/J-Link connection
- Board target: `nrf9161dk/nrf9161/ns`

Firmware version:

- `0.1.0`

SDK version:

- nRF Connect SDK v3.3.0

Steps:

1. Build firmware with `CONFIG_PET_TRACKER_BUZZER=y`.
2. Keep `CONFIG_PET_TRACKER_BUZZER_STARTUP_MELODY=y` for the first bring-up test.
3. Flash the firmware to the nRF9161 DK.
4. Reset the board.
5. Listen for the startup melody.
6. Observe UART logs.

Expected result:

- Buzzer emits the startup rhythm during boot.
- UART log includes `Buzzer initialized`.
- UART log includes `Running buzzer startup melody`.
- UART log includes `Buzzer startup melody complete`.
- Device remains responsive after the pattern.

Actual result:

- Not run yet.

Notes:

- Prototype pin: Arduino D2 / P0.16.
- The Grove module is an active buzzer; the initial firmware test can use simple GPIO on/off control.

## TEST-HW-008: Vibration Motor

Hardware:

- nRF9161 DK
- Mini vibration motor wired through IRLZ44N per `docs/hardware/prototype-wiring.md`
- Flyback diode across the motor
- External 3.3 V rail from Pololu S7V8F3
- USB/J-Link connection
- Board target: `nrf9161dk/nrf9161/ns`

Firmware version:

- `0.1.0`

SDK version:

- nRF Connect SDK v3.3.0

Steps:

1. Build firmware with the prototype vibration GPIO enabled.
2. Flash the firmware to the nRF9161 DK.
3. Trigger a short vibration test burst.
4. Observe UART logs and confirm the DK does not reset.

Expected result:

- Motor vibrates for the expected duration.
- UART log records vibration start and completion.
- No brownout, reset, or repeated boot log occurs.

Actual result:

- Not run yet.

Notes:

- Prototype pin: Arduino D3 / P0.17.
- Keep early test bursts short, for example 100 ms to 500 ms.
- Do not connect the motor directly to an MCU GPIO.
