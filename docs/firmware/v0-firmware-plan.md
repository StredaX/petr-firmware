# V0 Firmware Plan

V0 is a bench bring-up firmware for the fixed prototype wiring in `docs/hardware/prototype-wiring.md`.

## Goals

- Prove the nRF9161 DK boots and logs reliably.
- Prove D2 / P0.16 can control the Grove active buzzer.
- Prove D3 / P0.17 can control the vibration motor through the MOSFET.
- Optionally prove the BMA400 is visible on Arduino I2C.
- Keep LTE, GNSS, MQTT, storage, and low-power policy out of V0 unless needed for a later milestone.

## Hardware Contract

| Function | Pin | Active Level | Firmware Name |
| --- | --- | --- | --- |
| Grove active buzzer SIG | Arduino D2 / P0.16 | High | `buzzer0` |
| Vibration motor MOSFET gate | Arduino D3 / P0.17 | High | `vibration0` |
| BMA400 SDA | Arduino I2C SDA / P0.31 | I2C | `bma400` |
| BMA400 SCL | Arduino I2C SCL / P0.30 | I2C | `bma400` |
| BMA400 INT | TBD | TBD | Not in V0 |

## Firmware Shape

Add the smallest driver layer around Zephyr GPIO APIs:

```text
src/
  drivers/
    buzzer.c
    buzzer.h
    vibration_motor.c
    vibration_motor.h
  platform/
    board_pins.c
    board_pins.h
```

Expected responsibilities:

- `board_pins`: read devicetree aliases or node labels for prototype pins.
- `buzzer`: configure buzzer GPIO as inactive output and expose `buzzer_on()`, `buzzer_off()`, `buzzer_beep_ms()`.
- `vibration_motor`: configure motor GPIO as inactive output and expose `vibration_motor_on()`, `vibration_motor_off()`, `vibration_motor_pulse_ms()`.
- `main`: keep boot/state logging, initialize V0 drivers, run a short optional startup melody, then return to slow heartbeat.

## Devicetree Plan

Create a board overlay for the app target and define GPIO nodes with aliases:

```dts
/ {
    aliases {
        buzzer0 = &pet_tracker_buzzer;
        vibration0 = &pet_tracker_vibration;
    };

    pet_tracker_outputs {
        compatible = "gpio-leds";

        pet_tracker_buzzer: buzzer {
            gpios = <&gpio0 16 GPIO_ACTIVE_HIGH>;
            label = "Pet Tracker Grove Buzzer";
        };

        pet_tracker_vibration: vibration {
            gpios = <&gpio0 17 GPIO_ACTIVE_HIGH>;
            label = "Pet Tracker Vibration Motor";
        };
    };
};
```

Keep BMA400 out of the first overlay unless its driver is added in the same task. Arduino I2C is already present in the generated DK devicetree as `i2c2` with P0.31/P0.30.

## Kconfig Plan

Add build-time switches so V0 tests do not run unexpectedly in later firmware:

```text
CONFIG_PET_TRACKER_V0_HARDWARE_TESTS
CONFIG_PET_TRACKER_BUZZER
CONFIG_PET_TRACKER_VIBRATION_MOTOR
CONFIG_PET_TRACKER_BUZZER_STARTUP_MELODY
```

Default behavior should be conservative:

- Buzzer and vibration drivers can be enabled for V0 builds.
- Boot startup melody should default to off after initial bench validation.
- Vibration pulse duration should be capped.

## Bring-Up Sequence

1. Build and flash existing boot firmware.
2. Wire only common GND and buzzer first.
3. Enable buzzer driver and run `TEST-HW-007`.
4. Wire MOSFET, motor, 220 ohm gate resistor, 10k pull-down, and 1N5819 diode.
5. Confirm external rail is 3.3 V with a multimeter before connecting motor positive.
6. Enable vibration driver and run `TEST-HW-008`.
7. Connect BMA400 to Arduino I2C and run a simple I2C/device-id check in a later V0.1 task.

## Test Pattern Limits

Recommended first patterns:

```text
buzzer:    about 1.6 s startup rhythm with short groups and a longer final tone
vibration: 200 ms on, 1 repeat
```

Do not use long repeating vibration loops in V0. They drain the battery and make power issues harder to diagnose.

## Definition Of Done

- Firmware builds for `nrf9161dk/nrf9161/ns`.
- Boot logs show successful driver initialization.
- Buzzer test emits the expected pattern.
- Vibration test produces one short pulse without resetting the DK.
- Manual test results are recorded in `docs/firmware/manual-tests.md`.
- Any pin changes are reflected in `docs/hardware/prototype-wiring.md`.
