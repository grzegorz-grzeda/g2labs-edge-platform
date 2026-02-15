# Firmware — G2Labs Edge Platform

This directory contains all **edge-side software** of the G2Labs Edge Platform.

The term *firmware* in this project refers to any software that runs directly on edge devices, including:

* MCU-based firmware (Zephyr, ESP-IDF)
* Linux-based edge agents (Raspberry Pi, Python)

The backend is intentionally **not part of this directory**.

---

# 🏗 Structure

```
firmware/
│
├── west.yml                 # Zephyr workspace manifest
├── zephyr/                  # Zephyr source (managed by west, gitignored)
│
├── modules/
│   └── g2labs-fw-common/    # Shared Zephyr module
│
├── apps/                    # Zephyr-based applications
│   ├── nrf52-gateway/
│   └── nrf52-sensor/
│
├── esp32/                   # ESP-IDF projects
│   ├── temp-node/
│   └── relay-node/
│
└── rpi/                     # Raspberry Pi edge agents (Python)
    ├── agents/
    └── libs/
```

---

# 🧠 Design Philosophy

The firmware layer is designed to:

* Be hardware-agnostic at the protocol level
* Follow a unified MQTT contract
* Support reproducible builds
* Keep Zephyr dependencies managed via west
* Allow mixed environments (MCU + Linux)

All edge devices publish telemetry using the same topic structure and payload format.

---

# 🔧 Zephyr (nRF52 / future nRF54)

Zephyr is managed using **west workspace**.

## Initial setup

From inside `firmware/`:

```bash
west init -l .
west update
west zephyr-export
```

Zephyr sources are downloaded into:

```
firmware/zephyr/
```

This directory is gitignored and version-pinned via `west.yml`.

## Build example

```bash
west build -b nrf52840dk/nrf52840 apps/nrf52-gateway
```

---

# 📦 Local Zephyr Module

Shared Zephyr code lives in:

```
modules/g2labs-fw-common/
```

This module is local-only and included using:

```
ZEPHYR_EXTRA_MODULES
```

It provides reusable components such as:

* Logging wrappers
* Device identity utilities
* MQTT serialization helpers
* Shared Kconfig options

---

# 🔌 ESP32 (ESP-IDF)

ESP32 projects are located in:

```
esp32/
```

Each subfolder is a standalone ESP-IDF project.

Build example:

```bash
idf.py build
idf.py flash
```

ESP32 nodes follow the same MQTT contract as Zephyr devices.

---

# 🐧 Raspberry Pi Edge Agents

Linux-based edge software lives in:

```
rpi/
```

These are typically:

* Python applications
* BLE scanners
* MQTT bridges
* Protocol converters
* Edge data preprocessors

Each agent is treated as a device in the MQTT topology and has its own:

* deviceId
* site
* telemetry/state topics

These agents are not managed by west.

---

# 📡 MQTT Contract

All firmware components must comply with the versioned topic structure:

```
v1/<site>/<deviceId>/telemetry
v1/<site>/<deviceId>/state
v1/<site>/<deviceId>/events
v1/<site>/<deviceId>/cmd
```

Devices must publish structured JSON payloads consistent with the shared schema.

---

# 🔐 Future Roadmap

Planned enhancements:

* MCUboot integration
* Secure MQTT configuration
* Device provisioning workflow
* Unified device identity library
* OTA update mechanism
* Signed firmware images

---

# 🎓 Educational Context

The firmware layer is structured to:

* Support training workshops
* Demonstrate cross-platform edge design
* Show clean separation of concerns
* Enable reproducible Zephyr builds
* Illustrate device/backend contract design

---

If you are using this repository for learning purposes, start with:

1. Zephyr application in `apps/`
2. MQTT contract documentation in `/docs`
3. Backend integration in `/apps/backend`
