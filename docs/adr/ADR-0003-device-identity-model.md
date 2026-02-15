# ADR-0003: Device Identity Model

**Status:** Accepted
**Date:** 2026-02-15
**Deciders:** G2Labs

---

## Context

G2Labs Edge Platform includes heterogeneous edge nodes:

* MCU devices (nRF52 via Zephyr, ESP32 via ESP-IDF)
* Linux edge agents (Raspberry Pi, Python)

All devices communicate via MQTT and must be addressable consistently by:

* The backend
* The frontend dashboard
* Operational tooling (logs, provisioning, OTA, diagnostics)

Without a stable identity model, common issues appear:

* Duplicated or shifting identifiers (MAC-based, ephemeral, user-chosen)
* Difficulty mapping sensor readings to real-world measurement points
* Problems detecting device replacement / re-provisioning
* Inconsistent MQTT topic paths

---

## Problem

We need a device identity scheme that is:

* Stable across reboots and firmware updates
* Unique within a deployment
* Usable across different platforms
* Easy to provision and troubleshoot
* Suitable for offline/local-first environments
* Compatible with future secure provisioning (certs/keys)

---

## Decision

The platform will use a **two-level identity model**:

1. **Device UID**: immutable, hardware-derived or manufacturer-derived identifier
2. **Device ID**: human-meaningful, deployment-scoped identifier used in MQTT topics and UX

Additionally, each device belongs to a **Site** (deployment namespace).

### Definitions

* **`siteId`**: identifies a local deployment (e.g., `bugriver-home`, `workshop-01`)
* **`deviceUid`**: stable low-level identity, ideally derived from hardware
* **`deviceId`**: stable logical identity within a site, used in MQTT topics and dashboard

---

## Identity Fields

### `siteId`

* String, lowercase, URL-safe
* Example: `bugriver-home`, `lab-a`, `workshop-01`

### `deviceUid` (immutable)

Primary purpose: reliably detect *the physical device*.

Preferred sources by platform:

* **nRF52 (Zephyr):** SoC unique ID / device-id API (if available) or FICR-based ID
* **ESP32:** eFuse MAC or chip ID
* **RPi:** CPU serial / machine-id (with explicit caveats)

Format:

* Hex or base32 string
* Example: `nrf52-0f12a9c3d4e5`, `esp32-a4c1382b91f0`

### `deviceId` (logical, user-facing)

Primary purpose: stable naming and addressing in the system.

* Must be unique **within `siteId`**
* Human readable (short, meaningful)
* Example: `kitchen-sensor-1`, `garden-gateway`, `ble-scanner-01`

---

## MQTT Topic Binding

MQTT topics use `siteId` + `deviceId` (not `deviceUid`):

```
v1/<siteId>/<deviceId>/telemetry
v1/<siteId>/<deviceId>/state
v1/<siteId>/<deviceId>/events
v1/<siteId>/<deviceId>/cmd
```

Rationale:

* Device replacement can keep the same `deviceId` by re-provisioning
* UX remains stable
* Topics remain stable for dashboards and rules

---

## Provisioning Strategy

Each device must have a persistent **device identity file/config** that contains:

* `siteId`
* `deviceId`
* `deviceUid` (read-only, derived at runtime if possible)
* optional: `provisioningVersion`, `keys`, `metadata`

### Storage by platform

* **Zephyr:** settings/NVS/flash partition (config struct or Kconfig defaults + runtime persisted values)
* **ESP32:** NVS partition
* **RPi:** config file on disk (e.g., `/etc/g2labs-edge/device.toml`)

---

## Operational Rules

1. **`deviceUid` never changes** for the same physical unit.
2. **`deviceId` may change** if the user renames the device or reprovisions it.
3. Backend stores mapping:
   `siteId + deviceId → current deviceUid` (and history if needed).
4. On connection/startup, devices publish a **state/hello** message including `deviceUid` and `fwVersion`.
5. If backend detects that `deviceId` is now reported by a different `deviceUid`, it marks it as a **replacement event** requiring operator confirmation (policy decision).

---

## Rationale

### 1️⃣ Human-friendly UX

Dashboards and workshops must use names like `kitchen-sensor-1`, not raw MACs.

### 2️⃣ Hardware replacement support

If a device dies, the new one can be provisioned to the same `deviceId` and the system stays consistent.

### 3️⃣ Multi-platform consistency

Works for MCU and Linux agents without assuming a single ID source.

### 4️⃣ Security evolution

`deviceUid` can later anchor:

* certificates
* pre-shared keys
* enrollment workflows

---

## Consequences

### Positive

* Stable addressing in MQTT and UI
* Clear separation of physical vs logical identity
* Easier provisioning and troubleshooting
* Supports device replacement cleanly
* Ready for future secure identity schemes

### Negative

* Requires a provisioning step (assign `siteId`, `deviceId`)
* Backend must handle conflict cases (same `deviceId` from different `deviceUid`)
* RPi identity sources may be less strict (must be handled carefully)

---

## Alternatives Considered

### A) Use only MAC/chip ID as device identity

Rejected because:

* Not human-friendly
* Not stable for device replacement under same logical role
* Harder for training materials and dashboards

### B) Use only user-assigned `deviceId`

Rejected because:

* No physical binding
* Collisions are easy
* Weak basis for security

### C) Backend-generated identity at first contact

Rejected (for now) because:

* Adds complexity for offline-first labs
* Requires additional handshake protocol

---

## Implementation Notes

### Required fields in device “hello/state”

Devices should publish on:

```
v1/<siteId>/<deviceId>/state
```

Minimum recommended payload:

```json
{
  "ts": "2026-02-15T15:12:01Z",
  "deviceUid": "esp32-a4c1382b91f0",
  "fwVersion": "0.1.0",
  "hw": { "platform": "esp32", "model": "esp32-devkitc" },
  "capabilities": ["telemetry", "cmd"]
}
```

---

## Summary

The platform adopts a two-level identity model:

* **`deviceUid`**: immutable physical identity
* **`deviceId`**: logical, user-facing identity used in MQTT and UX
* **`siteId`**: namespace for deployments

MQTT topics bind to `siteId + deviceId`, while backend tracks `deviceUid` for physical verification and future security.
