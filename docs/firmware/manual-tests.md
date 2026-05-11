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
2. Enable `CONFIG_PET_TRACKER_BUZZER_STARTUP_TONE=y` only for the buzzer bring-up test.
3. Flash the firmware to the nRF9161 DK.
4. Reset the board.
5. Listen for the startup tone.
6. Observe UART logs.

Expected result:

- Buzzer emits one short startup tone during boot.
- UART log includes `Buzzer initialized`.
- UART log includes `Running buzzer startup tone`.
- UART log includes `Buzzer startup tone complete`.
- Device remains responsive after the pattern.

Actual result:

- Not run yet.

Notes:

- Prototype pin: Arduino D2 / P0.16.
- The Grove module is an active buzzer; the initial firmware test can use simple GPIO on/off control.
- Keep `CONFIG_PET_TRACKER_BUZZER_STARTUP_TONE` disabled outside manual buzzer bring-up.

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

1. Confirm the motor circuit matches `docs/hardware/prototype-wiring.md`.
2. Confirm the external rail is about 3.3 V with a multimeter.
3. Build firmware with `CONFIG_PET_TRACKER_VIBRATION_MOTOR=y`.
4. For this test only, enable `CONFIG_PET_TRACKER_VIBRATION_BOOT_TEST=y`.
5. Flash the firmware to the nRF9161 DK.
6. Reset the board.
7. Observe one short vibration pulse.
8. Observe UART logs and confirm the DK does not reset.

Expected result:

- Motor vibrates for the configured diagnostic pulse pattern.
- UART log includes `Vibration motor initialized`.
- UART log includes `Running vibration motor boot test`.
- UART log includes `Vibration motor boot test pulse`.
- UART log includes `Vibration motor boot test complete`.
- No brownout, reset, or repeated boot log occurs.

Actual result:

- Passed on 2026-05-12. Vibration motor works on the V0 prototype.

Notes:

- Prototype pin: Arduino D3 / P0.17.
- During troubleshooting, use longer pulses so Gate voltage can be measured.
- After bring-up, return to short pulses, for example 100 ms to 500 ms.
- Do not connect the motor directly to an MCU GPIO.
- Keep `CONFIG_PET_TRACKER_VIBRATION_BOOT_TEST` disabled outside manual bring-up.

Troubleshooting if the motor does not vibrate:

1. Measure Gate to GND during a boot-test pulse. Expected: about 3 V.
2. Measure Motor+ to GND. Expected: about 3.3 V from the external rail.
3. Measure Source to GND. Expected: 0 V.
4. Check IRLZ44N pinout with text facing you and legs down: left Gate, middle Drain, right Source.
5. Check the flyback diode direction: stripe/cathode to Motor+, other side to Motor-/Drain.
6. Briefly test the motor directly from the 3.3 V rail and GND to confirm the motor itself works.
7. Confirm nRF9161 DK GND and external rail GND are connected.
