# 🐾 Pet Tracker — Full Specification

## 1. Product Vision

Pet Tracker with Behavioral AI.

The product is not only a GPS tracker. It should understand pet behavior, detect anomalies, calculate risk, and warn the owner before a situation becomes critical.

## 2. Core Concept

- Personal behavior baseline
- Location tracking
- Activity tracking
- Anomaly detection
- Risk scoring
- Prediction and alerts

## 3. High-Level Architecture

```text
Device nRF9161
   ↓ MQTT over LTE
EMQX MQTT Broker
   ↓
Backend Worker / AI Engine
   ↓
PostgreSQL / Redis / Alerts

Mobile App / Web Portal
   ↓ HTTPS
Backend API
```

## 4. Main Components

- Firmware device based on nRF9161 DK
- MQTT broker based on EMQX Open Source
- Backend API based on ASP.NET Core Minimal API
- Backend Worker for telemetry processing, alerts, commands, and AI scoring
- PostgreSQL for persistent data
- Redis for cache, queues, and short-lived state
- Next.js frontend portal
- Kotlin Multiplatform mobile app later

## 5. Hardware MVP

- nRF9161 DK
- SIM card for LTE-M / NB-IoT
- BMA400 accelerometer, planned
- Vibration motor via MOSFET, planned
- Grove buzzer, planned
- Battery temperature sensor, planned
- Breadboard and jumper wires

## 6. Telemetry

Telemetry payload should support:

- Device ID
- Timestamp
- GNSS location
- Battery level
- Battery temperature
- Device state
- Activity level
- Motion data
- Firmware version
- Network status

## 7. Commands

Supported downlink commands:

- Trigger buzzer
- Trigger vibration
- Request location
- Update config
- Reboot device
- Start firmware update, future
- Change telemetry interval

## 8. Safe Zones

MVP:

- Radius-based safe zone
- Exit detection
- Entry detection
- Alert on breach

Future:

- Polygon safe zones
- Multiple safe zones per pet
- Indoor zones with BLE beacons

## 9. Behavioral AI

MVP AI should be rule-based.

Initial signals:

- Activity baseline
- Sleep/activity pattern
- Unusual inactivity
- Unusual night activity
- Safe zone breach
- Low battery risk
- Lost connection risk

Risk levels:

```text
LOW
MEDIUM
HIGH
CRITICAL
```

## 10. OTA Strategy

MVP:

- USB/J-Link flashing during development

Future:

- BLE DFU via companion nRF52840
- LTE FOTA later
- Rollback support
- Firmware validation

## 11. Connectivity Strategy

Primary device communication:

```text
MQTT over TLS
```

Backend and portal communication:

```text
HTTPS REST API
```

## 12. Security

- MQTT over TLS
- Per-device credentials
- ACL per device
- Device authentication
- Secure firmware validation
- Backend JWT authentication
- Role-based access control for portal

## 13. Roadmap

```text
1. nRF9161 bring-up
2. Firmware skeleton
3. MQTT connectivity
4. Backend API
5. EMQX integration
6. Telemetry ingestion
7. Safe zone detection
8. Alerts
9. Basic rule-based AI
10. BMA400 activity tracking
11. Portal dashboard
12. BLE companion
13. BLE DFU
14. Custom PCB
```

## 14. MVP Success Criteria

- Device boots and logs correctly
- Device connects to LTE
- Device publishes MQTT telemetry
- Backend stores telemetry
- Portal displays device and pet state
- Safe zone alerts work
- Commands can be sent to device
- Basic behavioral anomaly detection works
