# ESP-DashboardPlus

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Library-orange?logo=platformio)](https://platformio.org/lib/show/ESP-DashboardPlus)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![ESP32](https://img.shields.io/badge/ESP32-Supported-green.svg)](https://www.espressif.com/en/products/socs/esp32)
[![ESP32 S3](https://img.shields.io/badge/ESP32S3-Supported-green.svg)](https://www.espressif.com/en/products/socs/esp32-s3)
[![ESP32 C3](https://img.shields.io/badge/ESP32C3-Supported-green.svg)](https://www.espressif.com/en/products/socs/esp32-c3)

A real-time, on-device web dashboard library for ESP32 microcontrollers. Create beautiful, responsive dashboards with minimal code using WebSocket-based communication.

<p align="center">
  <img src="docs/assets/dashboard-preview.png" alt="ESP Dashboard Preview" width="800">
</p>

## ✨ Features

- **🔌 Real-time Updates** - WebSocket-based communication for instant UI updates
- **📊 14 Card Types** - Stats, charts, gauges, toggles, sliders, buttons, and more
- **🎨 Modern Dark & Light Theme** - Industrial IoT aesthetic with light/dark mode support
- **📱 Responsive Design** - Works on desktop and mobile browsers
- **🗂️ Tabbed Interface** - Dashboard, Console, and OTA tabs in a single HTML file
- **🔄 OTA Updates** - Dedicated firmware update tab with device info
- **📝 Console Logging** - Full-page console tab with filtering, export, and command input
- **⚡ Optimized** - Gzip-compressed HTML stored in PROGMEM (~18KB)
- **🛠️ Easy Integration** - Simple C++ API with minimal boilerplate

> **Note**: OTA and Console are available as **tabs only**, not dashboard cards. Configure visibility with `enableOTA` and `enableConsole` parameters in `begin()`.

<p align="center">
  <img src="docs/assets/console.png" alt="Interactive Console" width="800">
</p>
<p align="center">
  <img src="docs/assets/update.png" alt="OTA Updates" width="800">
</p>

## 📦 Installation

### PlatformIO (Recommended)

Add to your `platformio.ini`:

```ini
lib_deps = 
    ESP-DashboardPlus
```

Or install via PlatformIO CLI:

```bash
pio lib install "ESP-DashboardPlus"
```

### Manual Installation

1. Download the latest release
2. Extract to your project's `lib/` folder
3. Include the library in your code

## 🚀 Quick Start

```cpp
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "ESPDashboardPlus.h"
"dashboard_html.h"  // Auto-generated

AsyncWebServer server(80);
ESPDashboardPlus dashboard("My Device");

void setup() {
    Serial.begin(115200);
    WiFi.begin("SSID", "PASSWORD");
    while (WiFi.status() != WL_CONNECTED) delay(500);
    
    // Initialize dashboard
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE);
    
    // Add a temperature display
    StatCard* temp = dashboard.addStatCard("temp", "Temperature", "25.0", "°C");
    
    // Add a toggle switch
    ToggleCard* led = dashboard.addToggleCard("led", "LED", "Status", false);
    led->onChange = [](bool state) {
        digitalWrite(LED_BUILTIN, state);
    };
    
    server.begin();
    Serial.printf("Dashboard: http://%s\n", WiFi.localIP().toString().c_str());
}

void loop() {
    dashboard.loop();
    
    // Update values periodically
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 2000) {
        lastUpdate = millis();
        dashboard.updateStatCard("temp", String(random(20, 30)));
    }
}
```

## 📊 Available Card Types

| Card | Description | Use Case |
|------|-------------|----------|
| **StatCard** | Display numeric values with units | Temperature, humidity, voltage |
| **GaugeCard** | Circular gauge with thresholds | CPU usage, battery level |
| **ChartCard** | Line/area/bar/scatter/step charts | Historical data visualization |
| **ToggleCard** | On/off switch | LED control, relay control |
| **SliderCard** | Range slider | Brightness, volume, PWM |
| **ButtonCard** | Simple clickable button | Trigger actions |
| **ActionButton** | Button with confirmation popup | Restart, factory reset |
| **LinkCard** | URL redirect button | External links, documentation |
| **InputCard** | Text/number input field | WiFi SSID, configuration |
| **DropdownCard** | Select menu | Mode selection, options |
| **ColorPickerCard** | Color picker with presets | RGB LED control |
| **DateCard** | Date/time picker | Scheduling, alarms |
| **TimezoneCard** | Browser timezone detection | Time synchronization |
| **StatusCard** | Icon + status message | Connection status, system health |

### Tabs (not cards)

| Tab | Description |
|-----|-------------|
| **Console** | Timestamped logging with filtering, export, and command input |
| **OTA Update** | Firmware update via drag-and-drop with progress display |

## 📖 Documentation

Full documentation is available at: **[https://aaronbeckmann.github.io/ESP-DashboardPlus](https://aaronbeckmann.github.io/ESP-DashboardPlus)**

- [Getting Started](https://aaronbeckmann.github.io/ESP-DashboardPlus/getting-started)
- [Card Reference](https://aaronbeckmann.github.io/ESP-DashboardPlus/cards)
- [API Reference](https://aaronbeckmann.github.io/ESP-DashboardPlus/api)
- [Examples](https://aaronbeckmann.github.io/ESP-DashboardPlus/examples)

## 📁 Library Structure

```
ESP-DashboardPlus/
├── library.json              # PlatformIO manifest
├── README.md                 # This file
├── LICENSE                   # MIT License
├── src/
│   ├── ESPDashboardPlus.h    # Main library header
│   └── dashboard_html.h      # Auto-generated (gzipped HTML)
├── extras/
│   └── dashboard.html        # Source HTML file
├── scripts/
│   ├── html_to_header.py     # Standalone HTML converter
│   └── html_to_header_pio.py # PlatformIO pre-build script
├── examples/
│   └── basic/
│       └── main.cpp          # Complete example
└── docs/                     # GitHub Pages documentation
    ├── index.md
    ├── getting-started.md
    ├── cards.md
    └── ...
```

## 🔧 Build Configuration

### platformio.ini

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200

lib_deps = 
    ESP-DashboardPlus

; HTML auto-conversion is handled by the library
```

### Manual HTML Conversion

If you modify `extras/dashboard.html`, regenerate the header:

```bash
python scripts/html_to_header.py extras/dashboard.html src/dashboard_html.h
```

## 🤝 Contributing

Contributions are welcome! Please read our [Contributing Guide](CONTRIBUTING.md) for details.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Built with [ESPAsyncWebServer](https://github.com/mathieucarbou/ESPAsyncWebServer)
- JSON handling by [ArduinoJson](https://arduinojson.org/)

## 📞 Support

- 📚 [Documentation](https://aaronbeckmann.github.io/ESP-DashboardPlus)
- 🐛 [Issue Tracker](https://github.com/aaronbeckmann/ESP-DashboardPlus/issues)
- 💬 [Discussions](https://github.com/aaronbeckmann/ESP-DashboardPlus/discussions)
