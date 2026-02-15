# ADR-0004: Unified MQTT Topic Model

**Status:** Accepted
**Date:** 2026-02-15
**Deciders:** G2Labs

---

## Context

G2Labs Edge Platform connects heterogeneous edge nodes:

* MCU devices (Zephyr/nRF52, ESP-IDF/ESP32)
* Linux edge agents (Raspberry Pi, Python)

Devices publish telemetry and state to a local MQTT broker. The backend ingests MQTT messages, validates payloads, stores data, and exposes APIs to the frontend.

Without a unified topic model:

* devices drift into ad-hoc topic naming
* backend parsing becomes fragile
* training materials become inconsistent
* future versioning and compatibility are difficult
* command/control semantics become unclear

We need a stable, versioned topic contract that is:

* easy to learn and teach
* consistent across platforms
* backward compatible by version
* suitable for offline/local-first deployments
* extensible for future features (OTA, provisioning, logs)

---

## Decision

The platform will use a **versioned, hierarchical MQTT topic model** based on:

* `protocolVersion`
* `siteId`
* `deviceId`
* `channel` (message purpose)

### Topic format

```
<protocolVersion>/<siteId>/<deviceId>/<channel>
```

Where:

* `protocolVersion` is a literal like `v1`
* `siteId` is the deployment namespace (ADR-0003)
* `deviceId` is logical device identity (ADR-0003)
* `channel` is one of the defined message channels below

---

## Channels

### `telemetry`

High-frequency measurement data (sensor readings).

```
v1/<siteId>/<deviceId>/telemetry
```

### `state`

Low-frequency device state / heartbeat / “hello” message.

```
v1/<siteId>/<deviceId>/state
```

### `events`

Discrete events and notable occurrences (e.g., reboot, error, button press).

```
v1/<siteId>/<deviceId>/events
```

### `cmd`

Commands sent *to* the device.

```
v1/<siteId>/<deviceId>/cmd
```

### `cmd/ack`

Command acknowledgements from the device.

```
v1/<siteId>/<deviceId>/cmd/ack
```

### `cmd/res`

Command result payloads (optional, if larger or delayed responses are needed).

```
v1/<siteId>/<deviceId>/cmd/res
```

### `logs` (optional)

Structured logs emitted by the device (use sparingly on constrained MCUs).

```
v1/<siteId>/<deviceId>/logs
```

---

## Wildcards and Subscriptions

### Backend subscriptions (recommended)

Backend should subscribe to:

* All telemetry:

  ```
  v1/+/+/telemetry
  ```
* All state:

  ```
  v1/+/+/state
  ```
* All events:

  ```
  v1/+/+/events
  ```
* Optional: all command acknowledgements/results:

  ```
  v1/+/+/cmd/+
  ```

### Frontend subscriptions (via backend stream)

Frontend should not connect to MQTT directly in the default architecture.
Instead, it receives live data via backend (WebSocket/SSE).

---

## Rationale

### 1️⃣ Versioned protocol

Placing `v1` at the root enables:

* future breaking changes without ambiguity (`v2/...`)
* parallel operation of multiple versions during migration
* clear workshop materials and documentation

### 2️⃣ Site namespace

`siteId` provides clean separation between deployments:

* home vs workshop
* multiple labs
* future multi-site installations

### 3️⃣ Logical device identity

Using `deviceId` keeps topics human-friendly and stable even if hardware changes
(ADR-0003 maintains physical identity separately via `deviceUid` in payloads).

### 4️⃣ Clear channel semantics

Channel names define intent and expected behavior:

* `telemetry` = frequent measurements
* `state` = heartbeat + metadata
* `events` = discrete occurrences
* `cmd` = device control
* `cmd/ack|res` = explicit command lifecycle

This reduces ambiguity and backend complexity.

---

## Consequences

### Positive

* Simple, teachable topic structure
* Predictable backend ingestion
* Easy wildcard subscriptions
* Supports version evolution
* Works for MCU and Linux agents
* Clear command lifecycle patterns

### Negative

* Slightly more structured than ad-hoc topics
* Requires discipline (no custom topic strings in app layer)
* Command correlation requires payload fields (see below)

---

## Command Lifecycle

Commands are published to:

```
v1/<siteId>/<deviceId>/cmd
```

Devices must respond with at least an ACK:

```
v1/<siteId>/<deviceId>/cmd/ack
```

If a command produces data or completes later, device may publish:

```
v1/<siteId>/<deviceId>/cmd/res
```

### Command correlation

All command-related payloads must include a `cmdId` (UUID or short unique string).

---

## Naming Rules

### `siteId`

* lowercase
* `[a-z0-9-]+`
* example: `bugriver-home`, `workshop-01`

### `deviceId`

* lowercase
* `[a-z0-9-]+`
* example: `kitchen-sensor-1`, `garden-gateway`

### `channel`

* must be one of the defined channels above
* no custom channels without a new ADR

---

## Alternatives Considered

### A) Topic version at the end

Rejected because:

* complicates wildcards
* makes it harder to route by version
* increases parser complexity

### B) Include `deviceUid` in topic

Rejected because:

* not human-friendly
* breaks device replacement workflows
* reduces usability in workshops

### C) Device-specific topic trees

Rejected because:

* makes backend and docs complex
* hurts portability across platforms

---

## Enforcement

* No MQTT topic string literals in Application Layer (ADR-0002)
* Topic formatting lives in shared libraries:

  * Zephyr: `modules/g2labs-fw-common`
  * ESP32: `esp32/common`
  * RPi: `rpi/libs/g2labs_rpi_common`

---

## Summary

The platform adopts a unified MQTT topic model:

```
v1/<siteId>/<deviceId>/<channel>
```

With standard channels:

* telemetry
* state
* events
* cmd (+ cmd/ack, cmd/res)
* logs (optional)

This ensures consistent device behavior, predictable backend ingestion, and long-term extensibility.
