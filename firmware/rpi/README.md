# 📄 Raspberry Pi Edge Agents

This directory contains **Linux-based edge device software** for the G2Labs Edge Platform.

Unlike MCU firmware (Zephyr / ESP-IDF), these components run on Linux systems such as:

* Raspberry Pi Zero W
* Raspberry Pi 3/4
* Other ARM-based SBC devices

These applications are considered **edge devices**, not backend services.

---

# 🧠 Role in the Architecture

RPi agents:

* Collect data (e.g., BLE scanning)
* Bridge protocols (e.g., BLE → MQTT)
* Preprocess data locally
* Act as edge gateways
* Publish telemetry via MQTT

They follow the same MQTT contract as MCU devices.

This ensures that:

> The backend does not care whether a device is an MCU or a Linux system.

---

# 📂 Structure

```
rpi/
│
├── agents/
│   ├── ble-scanner/
│   ├── mqtt-bridge/
│   └── ...
│
├── libs/
│   └── g2labs_rpi_common/
│
└── systemd/
```

---

# 🧩 Agents

Each agent is a standalone device-like component.

Example:

```
agents/ble-scanner/
  pyproject.toml
  src/
  config.yaml
  README.md
  systemd/
```

Each agent:

* Has its own `deviceId`
* Connects to MQTT broker
* Publishes telemetry
* May subscribe to command topics
* Can run as a systemd service

---

# 📡 MQTT Contract

All RPi agents must publish using the versioned topic structure:

```
v1/<site>/<deviceId>/telemetry
v1/<site>/<deviceId>/state
v1/<site>/<deviceId>/events
v1/<site>/<deviceId>/cmd
```

Example telemetry payload:

```json
{
  "ts": "2026-02-15T15:12:01Z",
  "readings": [
    {
      "sensorId": "ble_temp_01",
      "type": "temperature",
      "value": 21.3,
      "unit": "C"
    }
  ]
}
```

---

# 🐍 Python Environment

Recommended tooling:

* Python 3.11+
* `uv` or `poetry` for dependency management
* Virtual environment per agent

Do not commit:

* `.venv/`
* cache files
* runtime logs

Each agent should include:

* `pyproject.toml`
* `requirements.lock` (or equivalent)
* `config.example.yaml`

---

# 🔐 Security Model (Planned)

Future improvements:

* Device authentication
* Secure MQTT (TLS)
* Credential provisioning workflow
* Local secret storage strategy

---

# 🎓 Educational Goals

The Raspberry Pi layer demonstrates:

* How Linux-based edge nodes integrate into MCU ecosystems
* Protocol bridging patterns
* Device identity abstraction
* Cross-platform MQTT architecture
* Edge-first system design

---

# 🚀 Example Agent Use Cases

* BLE temperature scanner (ATC thermometers)
* Zigbee to MQTT bridge
* OpenThread border router integration
* Modbus-to-MQTT bridge
* GPIO monitoring node
* Local data buffering before backend sync

---

If you are starting fresh, begin with:

```
agents/ble-scanner/
```

and implement:

* MQTT connection
* Device identity config
* Periodic telemetry publishing