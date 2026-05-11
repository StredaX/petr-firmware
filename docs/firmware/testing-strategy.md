# Firmware Testing Strategy

Pet Tracker firmware work should add the strongest practical tests for every feature as it is developed. Prefer automated tests for pure logic and repeatable integration behavior. Keep hardware-only behavior documented as manual or hardware-in-the-loop tests until a test rig exists.

## Testing Rule For New Work

Every non-trivial firmware change should answer:

```text
What can be tested automatically without hardware?
What needs a local integration dependency such as EMQX?
What needs the nRF9161 DK or real network/GNSS?
What manual test procedure records the hardware result?
```

If a feature has testable pure logic, add an automated test in the same development task. If a feature needs hardware, add or update a manual test in `docs/firmware/manual-tests.md`. If the feature changes MQTT payloads, commands, topics, or update states, update the relevant contract documentation too.

## Test Levels

| Level | Runs Where | Purpose | Required For |
| --- | --- | --- | --- |
| Build test | CI and local SDK shell | Prove firmware compiles for `nrf9161dk/nrf9161/ns`. | Every firmware code change. |
| Unit test | Host/native test target where practical | Pure logic without hardware, modem, GNSS, or broker. | State, config, payloads, commands, retry/backoff, OTA state logic. |
| Integration test with fakes | Host/native or Zephyr test app | Service behavior against fake providers. | MQTT service shape, GNSS/location service shape, command dispatch, offline queues. |
| Local broker/backend test | Developer machine or CI service container | Verify MQTT topics, payloads, ACKs, and backend-facing behavior. | Telemetry publish, command ACK, config subscription. |
| Hardware-in-the-loop | nRF9161 DK connected to test PC | Flash, UART assertions, GPIO behavior, real DK boot. | GPIO drivers, modem bring-up, DK-specific behavior. |
| Field/manual test | Real environment | LTE registration, GNSS fixes, power behavior, wearable behavior. | LTE/GNSS/power tests that cannot be made deterministic yet. |

## Feature-To-Test Matrix

| Feature Area | Automated Tests To Add | Manual/HIL Tests To Add |
| --- | --- | --- |
| State machine | Valid and invalid transitions, state names, error paths. | Boot log confirms expected transition sequence. |
| App config | Defaults, bounds, invalid config rejection. | Log confirms applied config on boot. |
| Telemetry payloads | Required fields, firmware version, device state, timestamp handling, JSON/schema compatibility. | MQTT subscriber observes payload on expected topic. |
| Commands | Parse valid/invalid commands, command IDs, ACK success/failure mapping. | Send command over MQTT and confirm ACK topic/payload. |
| MQTT service | Topic construction, reconnect/backoff decisions, QoS choices, offline behavior with fake client. | Local EMQX publish/subscribe test, later LTE broker test. |
| Location/GNSS | Location state mapping, timeout handling, stale fix handling using fake GNSS provider. | Outdoor GNSS fix test with timeout and UART/MQTT evidence. |
| BMA400/activity | Thresholds, activity classification, wake/inactivity mapping using sample data. | I2C detection and interrupt/wake-on-motion procedure. |
| Buzzer | Pattern timing table validation where practical. | `TEST-HW-007` startup tone on D2 / P0.16. |
| Vibration motor | Duration limits and command validation. | `TEST-HW-008` short pulse on D3 / P0.17 without reset. |
| Power/retry logic | Backoff intervals, sleep eligibility, pending-work gates. | Current draw and sleep/wake manual measurement. |
| OTA/update | Manifest parsing, version checks, battery preconditions, update state machine, rollback decisions. | USB/J-Link version A to version B check now; future MCUBoot/FOTA HIL. |
| Storage/offline queue | Enqueue/dequeue, capacity, persistence error handling, drop policy. | Power-cycle queue retention test when storage is implemented. |

## V0 Testing Scope

V0 should have:

```text
build for nrf9161dk/nrf9161/ns
manual boot/UART test
manual buzzer startup tone test
manual vibration motor pulse test
optional BMA400 I2C detection test
```

As soon as a test framework is added, V0 logic candidates are:

- app state transitions
- app config defaults
- buzzer startup pattern table
- vibration pulse duration validation

Initial automated test app:

```text
tests/app_core
```

Run:

```powershell
$toolchain = "C:\ncs\toolchains\936afb6332"
$env:ZEPHYR_BASE = "D:\ncs\v3.3.0\zephyr"
$env:ZEPHYR_TOOLCHAIN_VARIANT = "zephyr"
$env:ZEPHYR_SDK_INSTALL_DIR = "$toolchain\opt\zephyr-sdk"
$env:PATH = "$toolchain\opt\bin;$toolchain\opt\bin\Scripts;$env:PATH"
& "$toolchain\opt\bin\python.exe" "$env:ZEPHYR_BASE\scripts\twister" -T tests/app_core -p nrf9161dk/nrf9161/ns --build-only -O D:\tw\app_core
```

Native execution with `native_sim` requires a host compiler/toolchain. On the current Windows NCS setup, use `nrf9161dk/nrf9161/ns --build-only` unless host tests have been configured.

Use a short `-O` path on Windows because TF-M generated paths can exceed Windows path limits when Twister writes under the repository.

Current status:

```text
tests/app_core builds successfully for nrf9161dk/nrf9161/ns with --build-only.
native_sim execution is pending host toolchain setup.
```

## V1 Testing Scope

V1 should add automated tests around protocol and telemetry before hardware field tests:

- telemetry payload creation tests
- MQTT topic construction tests
- command parsing and ACK mapping tests
- retry/backoff tests
- local EMQX integration test for telemetry and command ACK
- manual LTE registration test
- manual GNSS fix test

## OTA Testing Scope

Current MVP update path is USB/J-Link flashing. Automated OTA-style tests should start with logic first:

- update manifest parsing
- version comparison
- battery/config precondition checks
- update state transitions
- update telemetry/event payloads
- rollback decision logic when MCUBoot support is added

Current hardware procedure:

```text
flash firmware version A
verify UART reports version A
flash firmware version B
verify UART reports version B
verify device returns to IDLE
```

Future LTE FOTA tests must be separated from normal CI because they depend on SIM, network quality, broker availability, battery level, and rollback configuration.

## CI Expectations

CI should eventually run:

```text
west build -b nrf9161dk/nrf9161/ns . --pristine
unit tests for pure logic
format/static checks if configured
local MQTT integration tests when they do not require real LTE
```

CI should not require:

```text
physical nRF9161 DK
SIM card
LTE network
GNSS signal
real production MQTT broker
```

Hardware-in-the-loop and field tests should be separate jobs or manual procedures until a stable lab rig exists.

## Documentation Requirement

When adding a feature:

- Add automated tests where practical.
- Add or update manual test procedures for hardware behavior.
- Record any hardware assumptions in `docs/hardware/prototype-wiring.md` or a successor hardware document.
- Update MQTT or OTA contracts when payloads, topics, commands, or update states change.
- In final development notes, report which tests were run and which were not run.
