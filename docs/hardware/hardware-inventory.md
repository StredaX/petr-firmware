# Hardware Inventory

This inventory records parts currently available for Pet Tracker prototypes and how they map to the staged hardware plan.

## Development Stages

| Version | Goal |
| --- | --- |
| V0 | Basic hardware bring-up on nRF9161 DK with buzzer, vibration motor, and optional BMA400 I2C detection. |
| V1 | LTE telemetry, GNSS, MQTT/command integration, and sensor data in payloads. |
| V2 | Battery-powered wearable prototype with charger, regulator, compact wiring, and power tuning. |
| V3 | Custom PCB and production-ready wearable hardware. |

## Main Platform

| Item | Purpose | Used In |
| --- | --- | --- |
| Nordic nRF9161 DK | Main firmware platform, LTE-M/NB-IoT, GNSS, GPIO, I2C. | V0, V1 |
| SIM card | LTE-M/NB-IoT development connectivity. | V1+ |

## Actuators

| Item | Purpose | V0 Decision |
| --- | --- | --- |
| Grove active buzzer 107020000 / Botland SEE-11298 | Audio alerts and feedback. | Connect directly to GPIO signal input on D2 / P0.16. |
| 3 V coin vibration motor / Botland DNG-17920 | Haptic alerts and feedback. | Drive only through MOSFET low-side switch on D3 / P0.17. |

## Motor Driver And Protection

| Item | Purpose | V0 Decision |
| --- | --- | --- |
| IRLZ44N N-MOSFET | Motor low-side switch and GPIO isolation. | Use for breadboard prototype; replace with smaller logic-level MOSFET for wearable PCB. |
| 220 ohm resistor | MOSFET gate series resistor. | Required between GPIO and gate. |
| 10k resistor | MOSFET gate pull-down. | Required between gate and GND. |
| 1N5819 Schottky diode | Preferred motor flyback protection. | Use across motor, stripe/cathode toward motor positive. |
| 1N4007 diode | Backup flyback protection. | Acceptable if 1N5819 is unavailable, but 1N5819 is preferred for low-voltage switching. |

## Sensors

| Item | Purpose | V0 Decision |
| --- | --- | --- |
| Grove BMA400 accelerometer / Botland SEE-14739 | Activity tracking, wake-on-motion experiments, behavior baseline. | Connect to Arduino I2C. Leave INT unassigned until wake-on-motion firmware. |

## Power

| Item | Purpose | Used In |
| --- | --- | --- |
| Akyga 1S Li-Po 3.7 V 620 mAh with JST-BEC | Portable prototype battery. | V2+; optional external rail test in V0. |
| TP4056 USB-C charger with protection | Li-Po charging and protection. | V2+; not needed for first V0 bench bring-up. |
| Pololu S7V8F3 3.3 V step-up/step-down regulator | Stable 3.3 V rail from Li-Po. | V2+; optional V0 external rail for motor tests. |

## Wiring And Assembly

| Item | Purpose | Used In |
| --- | --- | --- |
| 400-hole breadboard | V0/V1 wiring prototype. | V0, V1 |
| Jumper wires | Breadboard and DK header wiring. | V0, V1 |
| Silicone wire, 26 AWG / 0.14 mm2 | Flexible power and wearable wiring. | V2+ |
| JST-PH connector kit | Modular battery/peripheral wiring. | V2+ |
| JST-BEC 2-pin plug with wire | Battery adapter wiring. | V2+ |
| Perfboard / prototype board | Semi-permanent assembly after breadboard validation. | V1/V2 |
| Tact switches | GPIO and wake-up testing. | Optional V0/V1 |
| SN-01BM crimping tool | Small connector crimping. | V2+ |

## Mechanical

| Item | Purpose | Used In |
| --- | --- | --- |
| 30 cm x 20 cm x 3 mm acrylic plate | Stable portable prototype base. | V0/V1 |
| M3 nylon screws, spacers, washers | Mounting DK, breadboard, and modules. | V0+ |

## Tools

| Item | Purpose |
| --- | --- |
| Digital multimeter | Voltage checks, continuity, resistance, current debugging. |
| Soldering station | Wire, connector, and perfboard assembly. |
| Cynel Sn60Pb40 0.5 mm solder wire | Main solder for manual assembly. |
| RMA-223 no-clean flux | Improves solder flow. |
| Rosin flux | Basic wire and through-hole soldering. |
| Solder wick / desoldering braid | Cleaning pads and fixing bridges. |
| Desoldering pump | Removing excess through-hole solder. |
| Vacuum pickup tool | Optional future SMD handling. |

## Immediate V0 Scope

V0 uses only the smallest useful subset:

```text
nRF9161 DK
400-hole breadboard
jumper wires
Grove active buzzer
3 V vibration motor
IRLZ44N MOSFET
220 ohm gate resistor
10k gate pull-down resistor
1N5819 flyback diode
optional Grove BMA400 on I2C
multimeter
acrylic base and M3 spacers
```

V0 intentionally does not require LTE, GNSS, MQTT, battery-powered operation, charger integration, or wearable packaging.
