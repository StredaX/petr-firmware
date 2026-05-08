# Hardware Plan

## Current Hardware

- nRF9161 DK
- SIM card

## Planned MVP Hardware

- BMA400 accelerometer
- Vibration motor
- MOSFET driver for motor
- Grove buzzer
- Breadboard
- Jumper wires
- Battery temperature sensor

## Hardware Inventory

Available prototype components are tracked in:

```text
docs/hardware/hardware-inventory.md
```

## Prototype v0 Wiring

The fixed breadboard wiring for the current nRF9161 DK prototype is documented in:

```text
docs/hardware/prototype-wiring.md
```

Current prototype pin allocation:

```text
Arduino D2 / P0.16 -> Grove active buzzer SIG
Arduino D3 / P0.17 -> IRLZ44N gate for vibration motor low-side switch
Arduino I2C SDA    -> BMA400 SDA
Arduino I2C SCL    -> BMA400 SCL
```

## BMA400

BMA400 is an ultra-low-power accelerometer.

Reasons for selection:

- Very low power consumption
- Activity detection
- Inactivity detection
- Wake-up interrupt
- Good fit for wearable pet tracker
- Enough for activity/sleep/anomaly detection

Connection:

```text
BMA400 -> nRF9161
VCC    -> 3.3V
GND    -> GND
SDA    -> I2C SDA
SCL    -> I2C SCL
INT    -> GPIO interrupt
```

## Buzzer

```text
GPIO -> buzzer
```

## Vibration Motor

```text
GPIO -> MOSFET -> vibration motor
```

## Future Hardware

- nRF52840 companion for BLE
- BLE DFU support
- Microphone for bark/stress detection
- BLE indoor beacons
- Custom PCB
- Charging dock
