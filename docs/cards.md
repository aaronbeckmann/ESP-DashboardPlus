---
layout: default
title: Cards Reference
nav_order: 3
---

# Cards Reference

ESP DashboardPlus provides 16 different card types for building your dashboard. The dashboard features a **tabbed interface** with three main sections:

- **Dashboard** - Main view with all your sensor cards and controls
- **Console** - Full-page console with log filtering, export, and command input (tab only)
- **OTA Update** - Dedicated firmware update page with device info (tab only)

> **Note**: OTA and Console are available as **tabs only**, not dashboard cards. Use `enableOTA` and `enableConsole` in `begin()` to control visibility.

---

## Card Grouping

Cards can be organized into visual groups with section headers. If no groups are defined, all cards are displayed in a single flat grid without section headers.

### Creating Groups

```cpp
// Create a group and add cards to it
dashboard.addGroup("sensors", "Sensor Data");
dashboard.addCardToGroup("sensors", "temp");
dashboard.addCardToGroup("sensors", "humidity");

// Or create with initial cards
dashboard.addGroup("controls", "Device Controls", {"led", "brightness", "fan"});
```

### Group Methods

| Method | Parameters | Description |
|--------|------------|-------------|
| `addGroup()` | `id, title` | Create an empty group with a section title |
| `addGroup()` | `id, title, {cardIds...}` | Create a group with initial cards |
| `addCardToGroup()` | `groupId, cardId` | Add a card to an existing group |
| `removeCardFromGroup()` | `groupId, cardId` | Remove a card from a group |
| `removeGroup()` | `id` | Remove a group (cards remain, just ungrouped) |

### Behavior

- **With groups**: Cards in groups appear under their section headers; ungrouped cards appear under "Other"
- **Without groups**: All cards appear in a single grid with no section headers

---

## Card Sizing

Cards can span multiple grid columns and/or rows using the `setSize()`, `setSizeX()`, and `setSizeY()` methods. Default size is 1×1. On mobile screens (<640px), cards automatically resize to 1×1 for optimal display.

### Setting Card Size

```cpp
// Span 2 columns, 1 row (wide card)
ChartCard* wideChart = dashboard.addChartCard("chart", "Wide Chart", ChartType::LINE, 20);
wideChart->setSize(2, 1);

// Span 1 column, 2 rows (tall card)
GaugeCard* tallGauge = dashboard.addGaugeCard("gauge", "Tall Gauge", 0, 100, "%");
tallGauge->setSizeY(2);

// Span 2 columns, 2 rows (large card)
ChartCard* largeChart = dashboard.addChartCard("large-chart", "Large Chart", ChartType::AREA, 30);
largeChart->setSize(2, 2);
```

### Size Methods

| Method | Parameters | Description |
|--------|------------|-------------|
| `setSize()` | `int x, int y` | Set both column and row span |
| `setSizeX()` | `int x` | Set column span only |
| `setSizeY()` | `int y` | Set row span only |

### Example

```cpp
// Create a dashboard with mixed card sizes
ChartCard* tempChart = dashboard.addChartCard("temp-chart", "Temperature History", ChartType::LINE, 30);
tempChart->setSize(2, 1);  // Wide chart spanning 2 columns

StatCard* temp = dashboard.addStatCard("temp", "Temperature", "23.5", "°C");
// Default 1x1 size

StatCard* humidity = dashboard.addStatCard("humidity", "Humidity", "65", "%");
// Default 1x1 size

GaugeCard* cpu = dashboard.addGaugeCard("cpu", "CPU Usage", 0, 100, "%");
cpu->setSize(1, 2);  // Tall gauge spanning 2 rows
```

---

## Card Ordering (Weight)

Cards are displayed in ascending order by weight within their group (or globally if no groups). Lower weight values appear first. Default weight is 0.

### Setting Weight

```cpp
// Set weight directly on the card
StatCard* temp = dashboard.addStatCard("temp", "Temperature", "23.5", "°C");
temp->setWeight(10);  // Appears after cards with weight < 10

// Create cards with different weights for ordering
dashboard.addStatCard("humidity", "Humidity", "65", "%")->setWeight(20);
dashboard.addStatCard("pressure", "Pressure", "1013", "hPa")->setWeight(30);
```

### Weight Methods

| Method | Parameters | Description |
|--------|------------|-------------|
| `setWeight()` | `int weight` | Set the display order weight |
| `getWeight()` | - | Get the current weight value |

### Example with Groups and Weights

```cpp
// Create groups
dashboard.addGroup("sensors", "Sensor Data");
dashboard.addGroup("controls", "Controls");

// Add cards with weights (lower = first)
auto* temp = dashboard.addStatCard("temp", "Temperature", "23.5", "°C");
temp->setWeight(10);

auto* humidity = dashboard.addStatCard("humidity", "Humidity", "65", "%");
humidity->setWeight(20);

auto* led = dashboard.addToggleCard("led", "LED", "Enable", false);
led->setWeight(10);

auto* brightness = dashboard.addSliderCard("brightness", "Brightness", 0, 100);
brightness->setWeight(20);

// Add to groups
dashboard.addCardToGroup("sensors", "temp");
dashboard.addCardToGroup("sensors", "humidity");
dashboard.addCardToGroup("controls", "led");
dashboard.addCardToGroup("controls", "brightness");

// Result: Within "Sensor Data": temp (10), humidity (20)
//         Within "Controls": led (10), brightness (20)
```

## Tab Configuration

You can enable or disable the Console and OTA tabs during initialization:

```cpp
// Enable both OTA and Console tabs (default)
dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE, true, true);

// Disable OTA tab (only Dashboard and Console)
dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE, false, true);

// Disable Console tab (only Dashboard and OTA)
dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE, true, false);

// Dashboard only (no OTA or Console tabs)
dashboard.begin(&server, DASHBOARD_HTML_DATA, DASHBOARD_HTML_SIZE, false, false);
```

### Setting Version Info (for OTA tab)

```cpp
dashboard.setVersionInfo("1.0.0", "2024-01-15");
```

### Global Command Handler (for Console tab)

When users send commands from the Console tab, they're handled by a global callback:

```cpp
dashboard.onCommand([](const String& command) {
    if (command == "help") {
        dashboard.logInfo("Commands: help, status, reboot");
    } else if (command == "reboot") {
        ESP.restart();
    }
});
```

### Global Logging (to Console tab)

Log directly to the Console tab:

```cpp
// Log to Console tab
dashboard.logDebug("Debug message");
dashboard.logInfo("Info message");
dashboard.logWarning("Warning message");
dashboard.logError("Error message");

// Or with explicit level
dashboard.log(LogLevel::INFO, "Message");
```

---

## StatCard

Displays a numeric value with optional unit and trend indicator.

### Visual Representation

```
┌─────────────────────────────────────┐
│  Temperature                        │
│                                     │
│         23.5 °C                     │
│           ↑ +0.5°C                  │
└─────────────────────────────────────┘
```

### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `String` | Unique identifier for the card |
| `title` | `String` | Card title displayed at the top |
| `value` | `String` | The main value to display |
| `unit` | `String` | Unit suffix (e.g., "°C", "%", "V") |

### Properties

| Property | Type | Description |
|----------|------|-------------|
| `variant` | `CardVariant` | Color theme (PRIMARY, SUCCESS, WARNING, etc.) |
| `trend` | `String` | Trend direction: "up", "down", or "" |
| `trendValue` | `String` | Trend value text (e.g., "+0.5°C") |

### Methods

| Method | Parameters | Description |
|--------|------------|-------------|
| `setValue()` | `const String& val` | Update the displayed value |
| `setTrend()` | `const String& t, const String& val` | Set trend direction and value |
| `setVariant()` | `CardVariant v` | Change the color variant |

### Example

```cpp
StatCard* tempCard = dashboard.addStatCard(
    "temp",           // ID
    "Temperature",    // Title
    "23.5",          // Initial value
    "°C"             // Unit
);

tempCard->setVariant(CardVariant::PRIMARY);
tempCard->setTrend("up", "+0.5°C");

// Update value
dashboard.updateStatCard("temp", "24.0");
```

---

## GaugeCard

Displays a circular gauge with configurable thresholds.

### Visual Representation

```
┌─────────────────────────────────────┐
│  CPU Usage                          │
│                                     │
│          ╭─────╮                    │
│        ╱    65%  ╲                  │
│       ╱           ╲                 │
│      ●─────────────●                │
│      0%          100%               │
└─────────────────────────────────────┘
```

### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `id` | `String` | - | Unique identifier |
| `title` | `String` | - | Card title |
| `min` | `float` | 0 | Minimum value |
| `max` | `float` | 100 | Maximum value |
| `unit` | `String` | "%" | Unit suffix |

### Example

```cpp
GaugeCard* cpuGauge = dashboard.addGaugeCard(
    "cpu", "CPU Usage", 0, 100, "%"
);
cpuGauge->setThresholds(60, 85);  // Warning at 60%, danger at 85%
cpuGauge->setValue(45);

// Update
dashboard.updateGaugeCard("cpu", 72);
```

---

## ChartCard

Displays a time-series chart with multiple chart type options. Supports both single-series and multi-series data.

### Visual Representation

```
┌─────────────────────────────────────┐
│  Temperature History                │
│  ● Indoor  ● Outdoor                │
│     25 ┤      ●                     │
│     24 ┤   ●     ●    ●             │
│     23 ┤ ●         ●     ●          │
│     22 ┤                    ●       │
│        └────────────────────────    │
└─────────────────────────────────────┘
```

### Chart Types

| Type | Description |
|------|-------------|
| `LINE` | Connected line chart with round points |
| `AREA` | Filled area chart |
| `BAR` | Vertical bar chart |
| `SCATTER` | Scatter plot with round points |
| `STEP` | Step/staircase line chart |

### Single-Series Example

```cpp
ChartCard* tempChart = dashboard.addChartCard(
    "temp-chart",           // ID
    "Temperature History",  // Title
    ChartType::AREA,        // Chart type
    30                      // Keep 30 data points
);
tempChart->setVariant(CardVariant::PRIMARY);

// Add data points
dashboard.updateChartCard("temp-chart", 23.5);
dashboard.updateChartCard("temp-chart", 24.0);
```

### Multi-Series Example

```cpp
ChartCard* multiChart = dashboard.addChartCard(
    "multi-chart",
    "Indoor vs Outdoor",
    ChartType::LINE,
    30
);

// Add series with name and color
multiChart->addSeries("Indoor", "#00D4AA");   // Series 0
multiChart->addSeries("Outdoor", "#3B82F6");  // Series 1

// Update specific series by index
dashboard.updateChartCard("multi-chart", 0, 22.5);  // Indoor
dashboard.updateChartCard("multi-chart", 1, 18.2);  // Outdoor
```

### Series Methods

| Method | Parameters | Description |
|--------|------------|-------------|
| `addSeries()` | `name, color` | Add a new data series with name and hex color |
| `addDataPoint()` | `seriesIndex, value` | Add data point to specific series |

---

## ToggleCard

On/off switch for controlling boolean states.

### Visual Representation

```
┌─────────────────────────────────────┐
│  LED Control                        │
│                                     │
│  Onboard LED               [═══●]   │
│                               ON    │
└─────────────────────────────────────┘
```

### Example

```cpp
ToggleCard* ledToggle = dashboard.addToggleCard(
    "led-toggle", "LED Control", "Onboard LED", false
);

ledToggle->onChange = [](bool value) {
    digitalWrite(LED_BUILTIN, value);
    Serial.printf("LED: %s\n", value ? "ON" : "OFF");
};

// Programmatic update
dashboard.updateToggleCard("led-toggle", true);
```

---

## SliderCard

Range slider for numeric value input.

### Visual Representation

```
┌─────────────────────────────────────┐
│  Brightness                         │
│                                     │
│  ○────────────●──────────○    75%   │
│  0           50         100         │
└─────────────────────────────────────┘
```

### Example

```cpp
SliderCard* brightness = dashboard.addSliderCard(
    "brightness", "Brightness", 0, 100, 5, "%"
);
brightness->setValue(75);

brightness->onChange = [](int value) {
    analogWrite(LED_PIN, map(value, 0, 100, 0, 255));
};
```

---

## ButtonCard

Simple clickable button that triggers an action.

### Example

```cpp
ButtonCardImpl* saveBtn = dashboard.addButtonCard(
    "save", "Settings", "Save Configuration",
    []() {
        saveSettingsToEEPROM();
        Serial.println("Settings saved!");
    }
);
saveBtn->setVariant(CardVariant::PRIMARY);
```

---

## ActionButton

Button with a confirmation popup before executing.

### Visual Representation

```
┌─────────────────────────────────────┐
│  ⚠ Restart Device?                  │
│                                     │
│  The device will restart and        │
│  temporarily lose connection.       │
│                                     │
│    [Cancel]          [Confirm]      │
└─────────────────────────────────────┘
```

### Example

```cpp
ActionButton* restartBtn = dashboard.addActionButton(
    "restart",
    "Device Control",
    "Restart Device",
    "Restart Device?",
    "The device will restart and temporarily lose connection.",
    []() {
        ESP.restart();
    }
);
restartBtn->setVariant(CardVariant::WARNING);
```

---

## LinkCard

Button that redirects to an external URL.

### Example

```cpp
LinkCard* docsLink = dashboard.addLinkCard(
    "docs", "Documentation", "View Docs", "https://github.com/example/docs"
);
docsLink->setTarget("_blank");
docsLink->setVariant(CardVariant::INFO);
```

---

## InputCard

Text or number input field with submit functionality.

### Example

```cpp
// Text input
InputCard* ssidInput = dashboard.addInputCard(
    "wifi-ssid", "WiFi SSID", "Enter network name..."
);
ssidInput->onSubmit = [](const String& value) {
    Serial.printf("New SSID: %s\n", value.c_str());
};

// Number input
InputCard* intervalInput = dashboard.addInputCard(
    "interval", "Update Interval", "1000"
);
intervalInput->setNumberInput(100, 60000, 100, "ms");
```

---

## DropdownCard

Dropdown/select menu for choosing from predefined options.

### Example

```cpp
DropdownCardImpl* modeDropdown = dashboard.addDropdownCard(
    "wifi-mode", "WiFi Mode", "Select mode..."
);
modeDropdown->addOption("sta", "Station (Client)");
modeDropdown->addOption("ap", "Access Point");
modeDropdown->addOption("apsta", "AP + Station");
modeDropdown->setValue("sta");

modeDropdown->onChange = [](const String& value) {
    Serial.printf("Mode: %s\n", value.c_str());
};
```

---

## ColorPickerCard

Color picker with preset colors and custom hex input.

### Example

```cpp
ColorPickerCard* colorPicker = dashboard.addColorPickerCard(
    "led-color", "LED Color", "#00D4AA"
);

colorPicker->onChange = [](const String& color) {
    // Parse hex color and apply to RGB LED
    long rgb = strtol(color.substring(1).c_str(), NULL, 16);
    int r = (rgb >> 16) & 0xFF;
    int g = (rgb >> 8) & 0xFF;
    int b = rgb & 0xFF;
    setRGB(r, g, b);
};
```

---

## DateCard

Date and optional time picker.

### Example

```cpp
// Date only
DateCard* scheduleDate = dashboard.addDateCard(
    "schedule", "Schedule Date", false
);

// With time
DateCard* alarmTime = dashboard.addDateCard(
    "alarm", "Alarm Time", true
);
alarmTime->setRange("2024-01-01", "2024-12-31");
alarmTime->setCallback([](const String& value) {
    Serial.printf("Selected: %s\n", value.c_str());
});
```

---

## TimeCard

Time picker (HH:MM or HH:MM:SS format).

### Example

```cpp
// Time only (HH:MM)
TimeCard* wakeTime = dashboard.addTimeCard(
    "wake", "Wake Time", false
);

// With seconds (HH:MM:SS)
TimeCard* preciseTime = dashboard.addTimeCard(
    "precise", "Precise Time", true
);
preciseTime->setCallback([](const String& value) {
    Serial.printf("Time selected: %s\n", value.c_str());
});
```

---

## LocationCard

Browser geolocation picker for latitude/longitude.

### Example

```cpp
LocationCard* locCard = dashboard.addLocationCard(
    "location", "Device Location", "Get Current Location"
);

locCard->setCallback([](float lat, float lon) {
    Serial.printf("Location: %.6f, %.6f\n", lat, lon);
});

// Programmatic update
dashboard.updateLocationCard("location", 51.5074, -0.1278);
```

## TimezoneCard

Button to detect and retrieve browser timezone.

### Example

```cpp
TimezoneCard* tzCard = dashboard.addTimezoneCard(
    "timezone", "Browser Timezone", "Detect Timezone"
);

tzCard->setCallback([](const String& tz, int offset, const String& offsetStr) {
    Serial.printf("Timezone: %s (%s)\n", tz.c_str(), offsetStr.c_str());
});
```

---

## StatusCard

Displays a status icon with label and optional message.

### Status Icons

| Icon | Constant | Use |
|------|----------|-----|
| ✓ | `CHECK` | Success, operational |
| ✗ | `ERROR` | Error, failure |
| ⚠ | `WARNING` | Warning, caution |
| ℹ | `INFO` | Information |
| 📶 | `WIFI` | WiFi status |
| ⚡ | `POWER` | Power status |
| 🔄 | `SYNC` | Syncing |
| ☁ | `CLOUD` | Cloud connection |
| 🔒 | `LOCK` | Locked |
| 🔓 | `UNLOCK` | Unlocked |

### Example

```cpp
StatusCard* wifiStatus = dashboard.addStatusCard(
    "wifi-status", "WiFi", StatusIcon::WIFI
);

wifiStatus->setStatus(
    StatusIcon::WIFI, CardVariant::SUCCESS,
    "Connected", WiFi.localIP().toString()
);
```
