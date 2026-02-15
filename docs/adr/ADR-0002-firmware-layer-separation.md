# ADR-0002: Firmware Layer Separation Strategy

**Status:** Accepted
**Date:** 2026-02-15
**Deciders:** G2Labs

---

## Context

The G2Labs Edge Platform supports multiple device environments:

* MCU-based firmware (Zephyr on nRF52)
* MCU-based firmware (ESP-IDF on ESP32)
* Linux-based edge agents (Raspberry Pi, Python)

These environments differ significantly in:

* Operating system
* Toolchain
* Language
* Build system
* Resource constraints

A clear architectural strategy is required to:

* Avoid code duplication
* Maintain platform independence
* Ensure consistent MQTT behavior
* Support future expansion

---

## Problem

Without clear separation:

* Business logic may become platform-coupled
* MQTT handling may differ per device
* Device identity may be inconsistent
* OTA strategy may fragment
* Educational clarity will suffer

We must define a layered firmware architecture that applies across all device types.

---

## Decision

Firmware will be structured into three logical layers:

```
+----------------------------------+
| Application Layer                |
| (Device behavior, features)      |
+----------------------------------+
| Platform Abstraction Layer       |
| (OS / drivers / networking)      |
+----------------------------------+
| Shared Device Logic Layer        |
| (Protocol, identity, telemetry)  |
+----------------------------------+
```

---

## Layer Definitions

### 1️⃣ Application Layer

Responsibilities:

* Device-specific behavior
* Sensor reading logic
* Control algorithms
* Scheduling logic

Examples:

* BLE scanner (RPi)
* Temperature sensor node (nRF52)
* Relay controller (ESP32)

This layer must not:

* Hardcode MQTT topic structures
* Implement device identity logic
* Reimplement shared serialization

---

### 2️⃣ Platform Abstraction Layer (PAL)

Responsibilities:

* OS integration
* Drivers
* Network stack bindings
* Timers
* Storage
* Hardware access

Examples:

* Zephyr API usage
* ESP-IDF networking
* Python BLE libraries
* GPIO drivers

This layer isolates hardware and OS differences.

---

### 3️⃣ Shared Device Logic Layer

Responsibilities:

* MQTT topic formatting
* Telemetry schema construction
* Device identity model
* State reporting format
* Command parsing
* Version tagging

In Zephyr:

```
modules/g2labs-fw-common/
```

In ESP32:

```
esp32/common/
```

In RPi:

```
rpi/libs/g2labs_rpi_common/
```

This layer must remain platform-agnostic.

---

## Visual Architecture

Example for nRF52:

```
[ App: sensor node ]
        ↓
[ g2labs-fw-common ]
        ↓
[ Zephyr APIs ]
        ↓
[ Hardware ]
```

Example for RPi:

```
[ App: BLE scanner ]
        ↓
[ g2labs_rpi_common ]
        ↓
[ Python libraries ]
        ↓
[ Linux ]
```

---

## Rationale

### 1️⃣ Portability

Business logic can migrate between:

* nRF52
* ESP32
* RPi

With minimal changes.

---

### 2️⃣ Consistent MQTT Contract

All devices:

* Publish identical telemetry schema
* Use identical topic naming
* Follow versioned protocol rules

---

### 3️⃣ Testability

Shared logic can be:

* Unit-tested independently
* Validated without hardware
* Simulated in CI

---

### 4️⃣ OTA Compatibility

Separation ensures:

* Update logic can remain independent
* Bootloader integration is isolated
* Device identity persists across updates

---

### 5️⃣ Educational Clarity

Workshops can clearly demonstrate:

* What is hardware-dependent
* What is protocol-dependent
* What is application-specific

This reinforces architectural thinking.

---

## Consequences

### Positive

* Cleaner codebase
* Easier refactoring
* Better scalability
* Multi-platform consistency
* Professional portfolio quality

### Negative

* Slightly more upfront structure
* Requires discipline in layer boundaries
* More abstraction planning early

---

## Non-Goals

This ADR does not define:

* Specific MQTT payload schema (see future ADR)
* Specific OTA mechanism
* Specific security implementation

---

## Enforcement

The following rules must be respected:

* No MQTT topic string literals inside Application Layer
* No direct driver usage inside Shared Device Logic
* No hardware-specific logic inside shared modules
* Each platform must provide a minimal integration wrapper

---

## Future Evolution

This separation enables:

* RTOS migration
* Multi-core MCU support
* Edge AI extensions
* Unified provisioning system
* Cloud bridge integration

---

## Summary

Firmware across all supported platforms will follow a strict three-layer separation:

1. Application Layer
2. Platform Abstraction Layer
3. Shared Device Logic Layer

This ensures portability, clarity, and long-term scalability.
