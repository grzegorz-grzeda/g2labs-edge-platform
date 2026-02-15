# G2Labs Edge Platform

**Local-First Edge IoT Reference Architecture**

G2Labs Edge Platform is a full-stack IoT reference implementation designed to demonstrate how to build a local-first, production-inspired edge system combining:

* 🔧 MCU firmware (Zephyr / ESP-IDF)
* 🐧 Linux-based edge agents (Raspberry Pi, Python)
* 🧠 Local backend (Node.js)
* 📊 Web dashboard (frontend)
* 📡 MQTT-based device communication

This repository serves as:

* A portfolio project
* A reference architecture
* A foundation for technical workshops
* A blueprint for building scalable edge systems

---

# 🏗 Architecture Overview

The platform follows a **layered edge architecture**:

```
[ Devices ]
   ├── nRF52 (Zephyr)
   ├── ESP32 (ESP-IDF)
   └── Raspberry Pi (Python agents)

          ↓ MQTT

[ Local Backend ]
   ├── MQTT ingestion
   ├── Data validation
   ├── Database
   └── REST / WebSocket API

          ↓

[ Frontend Dashboard ]
```

### Core Principles

* Local-first operation (no cloud required)
* Clear separation of device vs backend responsibilities
* Explicit MQTT contract
* Versioned topic naming
* Device identity model
* Firmware-agnostic backend

---

# 📂 Repository Structure

```
g2labs-edge-platform/
│
├── apps/
│   ├── backend/              # Node.js local backend
│   └── frontend/             # Web dashboard
│
├── firmware/                 # Edge device software
│   ├── west.yml              # Zephyr workspace manifest
│   ├── zephyr/               # Zephyr (managed by west, gitignored)
│   │
│   ├── modules/
│   │   └── g2labs-fw-common/ # Shared Zephyr module
│   │
│   ├── apps/                 # Zephyr applications (nRF52, etc.)
│   │   ├── nrf52-gateway/
│   │   └── nrf52-sensor/
│   │
│   ├── esp32/                # ESP-IDF projects
│   │   ├── temp-node/
│   │   └── relay-node/
│   │
│   └── rpi/                  # Raspberry Pi edge agents (Python)
│       ├── agents/
│       │   ├── ble-scanner/
│       │   └── mqtt-bridge/
│       └── libs/
│
├── packages/
│   └── shared/               # Shared schemas / contracts
│
├── infra/
│   └── docker/               # Local infrastructure (MQTT, DB)
│
└── docs/
    ├── architecture/
    ├── adr/
    └── mqtt-contract/
```

---

# 🔌 Device Layer

The `firmware/` directory contains all **edge-side software**, regardless of hardware platform.

### MCU Firmware

* Zephyr-based (nRF52, future nRF54)
* Managed via west workspace
* Shared reusable module: `g2labs-fw-common`
* OTA-ready design (MCUboot planned)

### ESP32 Firmware

* ESP-IDF based projects
* MQTT-enabled nodes
* Telemetry + command support

### Raspberry Pi Edge Agents

* Python-based device software
* BLE scanning
* Protocol bridges
* Edge data preprocessing
* MQTT publishing

All edge devices follow the same MQTT contract.

---

# 📡 MQTT Communication Model

Topic structure (versioned):

```
v1/<site>/<deviceId>/telemetry
v1/<site>/<deviceId>/state
v1/<site>/<deviceId>/events
v1/<site>/<deviceId>/cmd
```

Telemetry payload example:

```json
{
  "ts": "2026-02-15T15:12:01Z",
  "readings": [
    {
      "sensorId": "temp1",
      "type": "temperature",
      "value": 21.3,
      "unit": "C"
    }
  ]
}
```

The backend validates and stores incoming data using shared schemas.

---

# 🧠 Backend Layer

The backend:

* Subscribes to MQTT
* Validates payloads
* Stores data
* Exposes REST API
* Streams live updates to frontend
* Manages users and roles

Designed for local deployment (Raspberry Pi or local server).

---

# 🎯 Project Goals

* Demonstrate clean IoT system architecture
* Provide reproducible Zephyr west workspace
* Show device/backend separation
* Enable hybrid MCU + Linux edge nodes
* Serve as training foundation

---

# 🚀 Getting Started (Firmware - Zephyr)

From `firmware/`:

```bash
west init -l .
west update
west zephyr-export
west build -b nrf52840dk/nrf52840 apps/nrf52-gateway
```

---

# 🧪 Development Mode (Local Backend)

Planned local stack:

* Mosquitto (MQTT broker)
* SQLite or PostgreSQL
* Node.js backend
* Docker-based setup

Details in `/infra`.

---

# 🔐 Security Roadmap

Planned topics:

* Device identity model
* Secure MQTT configuration
* OTA update mechanism
* Firmware signing
* Role-based backend access
* Secure remote access via VPS bridge

---

# 📚 Educational Purpose

This project is structured to support:

* Step-by-step workshops
* Architecture discussions (ADR-based)
* Multi-platform firmware development
* Real-world IoT patterns
* Local-first deployment strategies

---

# 📄 License

MIT License — see `LICENSE`.

---

# 👤 Author

Grzegorz Grzęda
G2Labs
Embedded Systems & IoT Engineering
