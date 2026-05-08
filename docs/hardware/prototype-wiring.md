# Prototype Wiring

This document fixes the breadboard wiring for the first Pet Tracker firmware prototype on the nRF9161 DK.

Board target:

```text
nrf9161dk/nrf9161/ns
```

Related docs:

```text
docs/hardware/hardware-inventory.md
docs/firmware/v0-firmware-plan.md
docs/firmware/manual-tests.md
```

## Prototype Parts

| Part | Prototype item | Notes |
| --- | --- | --- |
| Main board | nRF9161 DK | Powered from USB/J-Link during bring-up. |
| Buzzer | Grove active buzzer, Botland SEE-11298 | 3.3 V to 5 V active buzzer. A high GPIO level turns it on. |
| Vibration motor | Mini vibration motor 10 x 2.7 mm, Botland DNG-17920 | 3 V motor, about 80 mA idle current. |
| Motor switch | IRLZ44N N-MOSFET, Botland UCC-01950 | Prototype low-side switch. Oversized for final wearable PCB, but acceptable on breadboard. |
| External regulator | Pololu S7V8F3 3.3 V step-up/step-down | 3.3 V output from 2.7 V to 11.8 V input. |
| Battery | 1S Li-Po 3.7 V, 620 mAh, JST-BEC | Prototype external peripheral/motor supply. |
| Accelerometer | Grove BMA400, Botland SEE-14739 | I2C accelerometer for activity and wake-on-motion experiments. |
| Breadboard/wires | 400-hole breadboard and jumper wires | Keep motor and power wires short. |

Account-specific order-history links are not used as hardware references. Keep public product pages or datasheets in this document instead.

## Fixed Pin Allocation

| Function | nRF9161 DK header pin | nRF GPIO | Direction | Notes |
| --- | --- | --- | --- | --- |
| Buzzer signal | Arduino D2 | P0.16 | Output | Drives Grove buzzer SIG. |
| Vibration motor enable | Arduino D3 | P0.17 | Output | Drives MOSFET gate through a resistor. |
| BMA400 SDA | Arduino I2C SDA | P0.31 | I2C | From generated DK devicetree for Arduino I2C. |
| BMA400 SCL | Arduino I2C SCL | P0.30 | I2C | From generated DK devicetree for Arduino I2C. |
| BMA400 INT | TBD | TBD | Input interrupt | Assign when wake-on-motion firmware is added. |

D2 and D3 overlap with the Arduino serial pins in some board configurations. UART1 is disabled in the current generated devicetree, so they are available for this prototype. Revisit this allocation if Arduino serial is enabled later.

## Power Topology

Bring-up mode:

```text
USB/J-Link -> nRF9161 DK
1S Li-Po   -> Pololu S7V8F3 VIN/GND
S7V8F3 VOUT 3.3 V -> external prototype rail
S7V8F3 GND        -> common prototype GND
nRF9161 DK GND    -> common prototype GND
```

Use the S7V8F3 3.3 V rail for the vibration motor and optional external-module power during motor tests. Do not tie the S7V8F3 VOUT directly to the DK 3V3 rail while the DK is also powered from USB unless the board power configuration has been checked. A shared ground is required for GPIO control.

For the first bring-up, power the DK from USB/J-Link and power the external rail from the Li-Po through the S7V8F3. Turn off or disconnect the external rail when the DK is off to avoid back-powering the MCU through signal pins.

## Grove Active Buzzer

The Grove buzzer is an active buzzer module, not an 8 ohm speaker. It can be controlled directly from a GPIO signal input.

Connection:

```text
nRF9161 DK / external rail     Grove buzzer
--------------------------     ------------
3.3 V                      ->  VCC
GND                        ->  GND
Arduino D2 / P0.16         ->  SIG
```

Firmware default:

```text
inactive: GPIO low
active:   GPIO high
```

PWM can be used later for audible effects, but the MVP command can start with a simple on/off pattern.

## Vibration Motor

The vibration motor must not be connected directly to an nRF9161 GPIO. Drive it through the IRLZ44N as a low-side N-MOSFET switch.

IRLZ44N TO-220 pinout, front side facing you with text visible and leads pointing down:

```text
left   = Gate
middle = Drain
right  = Source
tab    = Drain
```

Connection:

```text
Arduino D3 / P0.17 --[220R]-- Gate
Gate ----------------[10k]---- GND

S7V8F3 VOUT 3.3 V ------------ Motor +
Motor - ---------------------- Drain
Source ----------------------- GND

Flyback diode across motor:
diode cathode ---------------- Motor +
diode anode ------------------ Motor - / Drain
```

Recommended parts around the MOSFET gate: 220 ohm series resistor from GPIO to gate and 10k pull-down from gate to GND. Recommended diode for the motor: 1N4148, 1N5819, SS14, or another small flyback diode rated above the motor current. The diode is required because the motor is an inductive load.

Firmware default:

```text
inactive: GPIO low
active:   GPIO high
```

Keep vibration bursts short during early tests, for example 100 ms to 500 ms, to reduce battery drain and motor heating.

## BMA400

Connection:

```text
nRF9161 DK / external rail     Grove BMA400
--------------------------     ------------
3.3 V                      ->  VCC
GND                        ->  GND
Arduino I2C SDA / P0.31    ->  SDA
Arduino I2C SCL / P0.30    ->  SCL
TBD GPIO interrupt         ->  INT
```

The interrupt pin is intentionally left unassigned until the wake-on-motion firmware task. Do not reuse D2 or D3 for BMA400 INT.

## Breadboard Layout Guidance

Use one breadboard rail as common GND and one rail as external 3.3 V from the S7V8F3. Place the MOSFET near the motor connection and keep the motor loop short:

```text
3.3 V rail -> motor -> MOSFET drain/source -> GND rail
```

Keep I2C wires away from the motor wiring where practical. If motor switching causes resets or noisy logs, add local capacitance across the external rail near the motor driver, for example 10 uF to 100 uF electrolytic plus 100 nF ceramic.

## References

- Grove active buzzer, Botland SEE-11298: https://botland.com.pl/grove-moduly-dzwiekowe/11298-grove-buzzer-modul-z-buzzerem-aktywnym-5903351246453.html
- Mini vibration motor 10 x 2.7 mm 3 V, Botland DNG-17920: https://botland.com.pl/silniki-wibracyjne/17920-mini-silnik-wibracyjny-10x27mm-3v-5904422300142.html
- IRLZ44N N-MOSFET, Botland UCC-01950: https://botland.com.pl/n-mosfet/1950-n-mosfet-irlz44n-55v47a-tht-5szt-5904422373276.html
- Pololu S7V8F3 3.3 V regulator: https://www.pololu.com/product/2122
- Grove BMA400 accelerometer, Botland SEE-14739: https://botland.com.pl/grove-akcelerometry-i-zyroskopy/14739-grove-3-osiowy-akcelerometr-cyfrowy-16-g-o-niskiej-mocy-bma400-5904422351816.html
