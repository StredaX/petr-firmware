# Firmware MQTT Contract

## Broker

```text
EMQX
MQTT over TLS
Port: 8883
```

## Client ID

```text
device-{deviceId}
```

## Publish Topics

```text
pt/v1/devices/{deviceId}/telemetry
pt/v1/devices/{deviceId}/state
pt/v1/devices/{deviceId}/events
pt/v1/devices/{deviceId}/commands/{commandId}/ack
```

## Subscribe Topics

```text
pt/v1/devices/{deviceId}/commands
pt/v1/devices/{deviceId}/config
```

## Telemetry Payload v0

```json
{
  "deviceId": "dev-001",
  "timestamp": "2026-05-06T12:00:00Z",
  "firmwareVersion": "0.1.0",
  "state": "ACTIVE",
  "batteryLevel": 87,
  "batteryTemperature": 31.2,
  "location": {
    "latitude": 51.1079,
    "longitude": 17.0385,
    "accuracy": 12.5
  },
  "activity": {
    "level": "LOW",
    "motionDetected": true
  },
  "network": {
    "type": "LTE-M",
    "rsrp": -92
  }
}
```

## Command Payload v0

```json
{
  "commandId": "cmd-001",
  "type": "REQUEST_LOCATION",
  "createdAt": "2026-05-06T12:00:00Z"
}
```

## Command Types

```text
REQUEST_LOCATION
TRIGGER_BUZZER
TRIGGER_VIBRATION
UPDATE_CONFIG
REBOOT
```
