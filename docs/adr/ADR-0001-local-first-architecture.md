# ADR-0001: Local-First Edge Architecture

**Status:** Accepted
**Date:** 2026-02-15
**Deciders:** G2Labs

---

## Context

The G2Labs Edge Platform is intended to:

* Serve as a reference IoT architecture
* Support educational workshops
* Combine MCU devices (Zephyr, ESP32) and Linux-based edge agents (Raspberry Pi)
* Demonstrate real-world system design patterns

A fundamental architectural decision must be made:

Should the system be:

1. Cloud-first (devices connect directly to remote cloud backend), or
2. Local-first (devices communicate with a local backend, optional cloud later)?

---

## Decision

The system will follow a **Local-First Edge Architecture**.

All devices:

* Communicate with a local MQTT broker
* Operate without internet access
* Use a locally deployed backend for data aggregation and management

Cloud connectivity, if added in the future, will be:

* Optional
* Implemented as a bridge layer
* Not required for core functionality

---

## Architecture Model

```
[ Edge Devices ]
   ├── nRF52 (Zephyr)
   ├── ESP32 (ESP-IDF)
   └── Raspberry Pi (Python agents)

          ↓

[ Local MQTT Broker ]

          ↓

[ Local Backend ]
   ├── Data validation
   ├── Storage
   ├── API
   └── User management

          ↓

[ Frontend Dashboard ]
```

Optional future layer:

```
Local Backend → Cloud Bridge → Remote Access
```

---

## Rationale

### 1️⃣ Reliability

Devices must function without internet access.

This is critical for:

* Home automation
* Industrial environments
* Field deployments
* Educational labs

---

### 2️⃣ Deterministic Behavior

Local systems:

* Avoid cloud latency
* Avoid cloud dependency
* Provide predictable system behavior

---

### 3️⃣ Security

Local-first reduces:

* Attack surface
* Cloud exposure
* External dependency risks

Security can be layered progressively:

* Local MQTT auth
* TLS
* Device identity provisioning
* Remote bridge with controlled access

---

### 4️⃣ Educational Clarity

For workshops and training:

* A local system is easier to understand
* No external infrastructure is required
* Participants can reproduce the system offline

---

### 5️⃣ Extensibility

Cloud features can be added later:

* MQTT bridge
* REST sync
* Remote management layer
* OTA distribution service

But the core remains local.

---

## Consequences

### Positive

* System works without internet
* Easier local debugging
* Clear separation of device and backend roles
* Reproducible environment
* Lower operational complexity

### Negative

* Remote access requires additional bridge layer
* Requires local infrastructure (broker + backend)
* Slightly more setup complexity compared to direct cloud SDK usage

---

## Alternatives Considered

### Cloud-First Architecture

Rejected because:

* Introduces external dependency
* Reduces offline usability
* Increases operational complexity
* Makes workshops dependent on cloud setup

---

### Hybrid from Day One

Rejected because:

* Increases initial architectural complexity
* Distracts from core educational goals

---

## Future Extensions

This decision does not prevent:

* Secure cloud bridge
* Remote monitoring
* Multi-site deployments
* Federated edge systems

The architecture is intentionally layered to support future evolution.

---

## Summary

G2Labs Edge Platform is designed as a **Local-First IoT Edge System**.

All core functionality must operate without cloud connectivity.

Cloud integration, if introduced, must remain optional and non-breaking.