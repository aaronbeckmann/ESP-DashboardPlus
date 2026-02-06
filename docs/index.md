---
layout: default
title: Home
nav_order: 1
---

# ESP-DashboardPlus

A real-time, on-device web dashboard library for ESP32 microcontrollers.

## Features

- **🔌 Real-time Updates** - WebSocket-based communication for instant UI updates
- **📊 16 Card Types** - Stats, charts, gauges, toggles, sliders, buttons, time/location pickers, and more
- **🎨 Modern Theme** - Industrial IoT aesthetic with light/dark mode support
- **📱 Responsive Design** - Works on desktop and mobile browsers
- **🗂️ Tabbed Interface** - Dashboard, Console, and OTA tabs in a single file
- **🔄 OTA Updates** - Dedicated firmware update tab (not a card)
- **📝 Console Logging** - Full-page console tab with filtering and command input
- **⚡ Optimized** - Gzip-compressed HTML stored in PROGMEM (~22KB)
- **📈 Multi-Series Charts** - Multiple data lines per chart with legends
- **📦 Card Groups** - Organize cards into groups
- **⚖️ Card Ordering** - Control display order with weight property
- **📐 Card Sizing** - Span cards across multiple grid columns/rows

> **Note**: OTA and Console are available as **tabs only**, not dashboard cards.

## Quick Start

```cpp
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "ESPDashboardPlus.h"
#include "dashboard_html.h"

AsyncWebServer server(80);
ESPDashboardPlus dashboard("My Device");

void setup() {
    WiFi.begin("SSID", "PASSWORD");
    while (WiFi.status() != WL_CONNECTED) delay(500);
    
    dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE);
    
    // Add cards
    dashboard.addStatCard("temp", "Temperature", "25.0", "°C");
    dashboard.addToggleCard("led", "LED", "Status", false);
    
    server.begin();
}

void loop() {
    dashboard.loop();
}
```

## Installation

### PlatformIO

Add to your `platformio.ini`:

```ini
lib_deps = 
    ESP-DashboardPlus
```

### Manual

1. Download the latest release from [GitHub](https://github.com/aaronbeckmann/ESP-DashboardPlus/releases)
2. Extract to your project's `lib/` folder
3. Include the library in your code

## Next Steps

- [Getting Started Guide](getting-started) - Complete setup tutorial
- [Cards Reference](cards) - All available card types
- [API Reference](api) - Complete API documentation
- [Examples](examples) - Code examples and demos
