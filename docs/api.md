---
layout: default
title: API Reference
nav_order: 4
---

# API Reference

Complete API documentation for ESP-DashboardPlus.

---

## ESPDashboardPlus Class

The main dashboard class that manages cards and WebSocket communication.

### Constructor

```cpp
ESPDashboardPlus(const String& title = "ESP32 Dashboard")
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `title` | `String` | "ESP32-DashboardPlus" | Dashboard title |

### Methods

#### begin()

Initialize the dashboard with PROGMEM HTML data.

```cpp
void begin(AsyncWebServer* server, const uint8_t* htmlData, size_t htmlSize, 
           bool enableOTA = true, bool enableConsole = true, const String& wsPath = "/ws")
```

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `server` | `AsyncWebServer*` | - | Pointer to web server |
| `htmlData` | `const uint8_t*` | - | PROGMEM HTML data |
| `htmlSize` | `size_t` | - | HTML data size |
| `enableOTA` | `bool` | true | Show the OTA Update tab |
| `enableConsole` | `bool` | true | Show the Console tab |
| `wsPath` | `String` | "/ws" | WebSocket path |

> **Note**: OTA and Console functionality are available as **tabs only**, not dashboard cards. Use the `enableOTA` and `enableConsole` parameters to control tab visibility.

> **Note on OTA support (ESP32)**  
> To use the OTA update tab reliably, your ESP32 must use a partition table that provides **OTA app slots** (e.g. `ota_0`, `ota_1`) with enough space for your firmware.  
> Common choices in Arduino / PlatformIO are partition schemes like `min_spiffs.csv` or other `*ota*.csv` variants that define at least one OTA partition.

**Example: PlatformIO configuration with OTA-capable partition table**

```ini
[env:esp32-s3-devkitm-1]
platform = espressif32
board = esp32-s3-devkitm-1
framework = arduino

; 4 MB flash with OTA-capable partition layout
board_upload.flash_size = 4MB
board_build.partitions = min_spiffs.csv

build_flags =
  -DARDUINO_USB_CDC_ON_BOOT=1
  -DBOARD_HAS_PSRAM
```

#### loop()

Process WebSocket events. Call this in your main loop.

```cpp
void loop()
```

#### setTitle()

Change the dashboard title and optionally subtitle.

```cpp
void setTitle(const String& title)
void setTitle(const String& title, const String& subtitle)
void setSubtitle(const String& subtitle)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `title` | `String` | Dashboard title displayed in header |
| `subtitle` | `String` | Subtitle displayed below title |

#### setVersionInfo()

Set firmware version info displayed in the OTA tab.

```cpp
void setVersionInfo(const String& version, const String& lastUpdate = "")
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `version` | `String` | Current firmware version |
| `lastUpdate` | `String` | Date of last update |

#### onCommand()

Set the global command handler for Console tab input.

```cpp
void onCommand(std::function<void(const String&)> handler)
```

#### isOTAEnabled()

Check if OTA tab is enabled.

```cpp
bool isOTAEnabled() const
```

#### isConsoleEnabled()

Check if Console tab is enabled.

```cpp
bool isConsoleEnabled() const
```

---

## Card Creation Methods

All methods return a pointer to the created card for further configuration.

### addStatCard()

```cpp
StatCard* addStatCard(const String& id, const String& title, 
                      const String& value = "", const String& unit = "")
```

### addGaugeCard()

```cpp
GaugeCard* addGaugeCard(const String& id, const String& title, 
                        float min = 0, float max = 100, const String& unit = "%")
```

### addChartCard()

```cpp
ChartCard* addChartCard(const String& id, const String& title, 
                        ChartType type = ChartType::LINE, int maxPoints = 20)
```

### addToggleCard()

```cpp
ToggleCard* addToggleCard(const String& id, const String& title, 
                          const String& label = "", bool defaultValue = false)
```

### addSliderCard()

```cpp
SliderCard* addSliderCard(const String& id, const String& title, 
                          int min = 0, int max = 100, int step = 1, 
                          const String& unit = "")
```

### addButtonCard()

```cpp
ButtonCardImpl* addButtonCard(const String& id, const String& title, 
                              const String& label, ButtonCallback cb)
```

### addActionButton()

```cpp
ActionButton* addActionButton(const String& id, const String& title, 
                              const String& label, const String& confirmTitle, 
                              const String& confirmMsg, ButtonCallback cb)
```

### addLinkCard()

```cpp
LinkCard* addLinkCard(const String& id, const String& title, 
                      const String& label, const String& url)
```

### addInputCard()

```cpp
InputCard* addInputCard(const String& id, const String& title, 
                        const String& placeholder = "")
```

### addDropdownCard()

```cpp
DropdownCardImpl* addDropdownCard(const String& id, const String& title, 
                                  const String& placeholder = "Select...")
```

### addColorPickerCard()

```cpp
ColorPickerCard* addColorPickerCard(const String& id, const String& title, 
                                    const String& defaultColor = "#00D4AA")
```

### addDateCard()

```cpp
DateCard* addDateCard(const String& id, const String& title, 
                      bool includeTime = false)
```

### addTimezoneCard()

```cpp
TimezoneCard* addTimezoneCard(const String& id, const String& title, 
                              const String& label = "Get Browser Timezone")
```

### addStatusCard()

```cpp
StatusCard* addStatusCard(const String& id, const String& title, 
                          StatusIcon icon = StatusIcon::INFO)
```

---

## Update Methods

Methods to update card values and broadcast changes to all connected clients.

### updateStatCard()

```cpp
void updateStatCard(const String& id, const String& value)
```

### updateGaugeCard()

```cpp
void updateGaugeCard(const String& id, float value)
```

### updateChartCard()

Update a single-series chart or the legacy data array:

```cpp
void updateChartCard(const String& id, float value)
```

Update a specific series in a multi-series chart:

```cpp
void updateChartCard(const String& id, int seriesIndex, float value)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `String` | Chart card ID |
| `value` | `float` | New data point value |
| `seriesIndex` | `int` | Index of the series to update (0-based) |

### updateToggleCard()

```cpp
void updateToggleCard(const String& id, bool value)
```

### updateSliderCard()

```cpp
void updateSliderCard(const String& id, int value)
```

### updateColorCard()

```cpp
void updateColorCard(const String& id, const String& color)
```

### updateDropdownCard()

```cpp
void updateDropdownCard(const String& id, const String& value)
```

### updateDateCard()

```cpp
void updateDateCard(const String& id, const String& value)
```

### updateLinkCard()

```cpp
void updateLinkCard(const String& id, const String& url)
```

### updateStatusCard()

```cpp
void updateStatusCard(const String& id, StatusIcon icon, CardVariant variant, 
                      const String& label, const String& message)
```

---

## Console Logging Methods

Log messages to the Console tab. Only works if `enableConsole = true` was set in `begin()`.

### logDebug(message)

```cpp
void logDebug(const String& message)
```

### logInfo(message)

```cpp
void logInfo(const String& message)
```

### logWarning(message)

```cpp
void logWarning(const String& message)
```

### logError(message)

```cpp
void logError(const String& message)
```

### log(level, message)

```cpp
void log(LogLevel level, const String& message)
```

---

## Card Management

### getCard()

Get a card by its ID.

```cpp
DashboardCard* getCard(const String& id)
```

### removeCard()

Remove a card from the dashboard.

```cpp
void removeCard(const String& id)
```

### setWeight()

Set the display order weight for a card. Cards with lower weight appear first within their group (or globally if no groups defined).

```cpp
card->setWeight(int weight)
```

### getWeight()

Get the current weight of a card.

```cpp
int weight = card->getWeight()
```

---

## Card Sizing

Control how many grid columns and rows a card spans. Default size is 1×1. On mobile screens (<640px), cards automatically resize to 1×1.

### setSize()

Set both column and row span at once.

```cpp
card->setSize(int sizeX, int sizeY)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `sizeX` | `int` | Number of columns to span (default: 1) |
| `sizeY` | `int` | Number of rows to span (default: 1) |

### setSizeX()

Set the column span only.

```cpp
card->setSizeX(int sizeX)
```

### setSizeY()

Set the row span only.

```cpp
card->setSizeY(int sizeY)
```

### Example

```cpp
// Wide chart spanning 2 columns
ChartCard* chart = dashboard.addChartCard("chart", "Temperature", ChartType::LINE, 20);
chart->setSize(2, 1);

// Large card spanning 2x2
ChartCard* large = dashboard.addChartCard("large", "Overview", ChartType::AREA, 30);
large->setSize(2, 2);

// Tall gauge spanning 2 rows
GaugeCard* gauge = dashboard.addGaugeCard("gauge", "CPU", 0, 100, "%");
gauge->setSizeY(2);
```

## Card Grouping

Organize cards into visual groups with section headers. Groups are optional - if no groups are defined, cards display in a flat grid without section titles.

### addGroup()

Create a card group with a section title.

```cpp
void addGroup(const String& id, const String& title)
void addGroup(const String& id, const String& title, std::initializer_list<String> cardIds)
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `id` | `String` | Unique group identifier |
| `title` | `String` | Section header displayed above cards |
| `cardIds` | `initializer_list` | Optional initial cards to include |

### addCardToGroup()

Add a card to an existing group.

```cpp
void addCardToGroup(const String& groupId, const String& cardId)
```

### removeCardFromGroup()

Remove a card from a group (card remains on dashboard, just ungrouped).

```cpp
void removeCardFromGroup(const String& groupId, const String& cardId)
```

### removeGroup()

Remove a group entirely (cards remain, just ungrouped).

```cpp
void removeGroup(const String& id)
```

---

## Enums

### CardVariant

```cpp
enum class CardVariant {
    PRIMARY,    // Teal/cyan
    SUCCESS,    // Green
    WARNING,    // Yellow/amber
    DANGER,     // Red
    INFO,       // Blue
    SECONDARY   // Gray
};
```

### ChartType

```cpp
enum class ChartType {
    LINE,      // Connected line with points
    AREA,      // Filled area
    BAR,       // Vertical bars
    SCATTER,   // Unconnected points
    STEP       // Staircase line
};
```

### StatusIcon

```cpp
enum class StatusIcon {
    CHECK,     // ✓
    ERROR,     // ✗
    WARNING,   // ⚠
    INFO,      // ℹ
    WIFI,      // 📶
    POWER,     // ⚡
    SYNC,      // 🔄
    CLOUD,     // ☁
    LOCK,      // 🔒
    UNLOCK     // 🔓
};
```

### LogLevel

```cpp
enum class LogLevel {
    DEBUG,     // Gray
    INFO,      // Blue
    WARNING,   // Yellow
    ERROR      // Red
};
```

---

## Callback Types

```cpp
typedef std::function<void()> ButtonCallback;
typedef std::function<void(const String&)> InputCallback;
typedef std::function<void(bool)> ToggleCallback;
typedef std::function<void(int)> SliderCallback;
typedef std::function<void(const String&)> ColorCallback;
typedef std::function<void(const String&)> DropdownCallback;
typedef std::function<void(const String&, int, const String&)> TimezoneCallback;
typedef std::function<void(const String&)> DateCallback;
```
