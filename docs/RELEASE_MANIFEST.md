# Release Manifest Format

Each GitHub release includes a `release.json` file that provides metadata about the release and all supported devices. This file is intended to be downloaded by applications to display available firmware versions and supported devices when flashing.

## Schema

```json
{
  "version": "string",
  "releaseDate": "string (ISO 8601)",
  "project": "string",
  "devices": [
    {
      "environment": "string",
      "name": "string",
      "displayName": "string",
      "firmwareFile": "string",
      "uploadConfig": {
        "protocol": "string",
        "mcu": "string",
        "speed": "number",
        "requires1200bpsTouch": "boolean"
      }
    }
  ]
}
```

## Fields

### Root Object

| Field | Type | Description |
|-------|------|-------------|
| `version` | string | Release version tag (e.g., "v2.1.0") |
| `releaseDate` | string | ISO 8601 timestamp of when the release was created |
| `project` | string | Project identifier, always "trenino" |
| `devices` | array | List of supported devices with their firmware files |

### Device Object

| Field | Type | Description |
|-------|------|-------------|
| `environment` | string | PlatformIO environment name used to build the firmware |
| `name` | string | Short identifier for the device (used in firmware filename) |
| `displayName` | string | Human-readable device name for display in UI |
| `firmwareFile` | string | Filename of the firmware binary in the release assets |
| `uploadConfig` | object | Upload configuration for flashing the device |

### Upload Config Object

| Field | Type | Description |
|-------|------|-------------|
| `protocol` | string | Upload protocol (e.g., "avr109", "arduino", "wiring", "sam-ba", "esptool") |
| `mcu` | string | Microcontroller unit identifier (e.g., "atmega32u4", "atmega328p", "esp32") |
| `speed` | number | Baud rate for upload (e.g., 57600, 115200, 921600) |
| `requires1200bpsTouch` | boolean | Whether the device requires a 1200bps touch on the serial port to enter bootloader mode |

## Example

```json
{
  "version": "v2.1.0",
  "releaseDate": "2025-01-15T10:30:00Z",
  "project": "trenino",
  "devices": [
    {
      "environment": "sparkfun_promicro16",
      "name": "sparkfun-pro-micro",
      "displayName": "SparkFun Pro Micro",
      "firmwareFile": "trenino-sparkfun-pro-micro.firmware.hex",
      "uploadConfig": {
        "protocol": "avr109",
        "mcu": "atmega32u4",
        "speed": 57600,
        "requires1200bpsTouch": true
      }
    },
    {
      "environment": "leonardo",
      "name": "arduino-leonardo",
      "displayName": "Arduino Leonardo",
      "firmwareFile": "trenino-arduino-leonardo.firmware.hex",
      "uploadConfig": {
        "protocol": "avr109",
        "mcu": "atmega32u4",
        "speed": 57600,
        "requires1200bpsTouch": true
      }
    },
    {
      "environment": "esp32dev",
      "name": "esp32",
      "displayName": "ESP32 DevKit",
      "firmwareFile": "trenino-esp32.firmware.bin",
      "uploadConfig": {
        "protocol": "esptool",
        "mcu": "esp32",
        "speed": 921600,
        "requires1200bpsTouch": false
      }
    }
  ]
}
```

## Supported Devices

| Name | Display Name | Firmware Extension |
|------|--------------|-------------------|
| `sparkfun-pro-micro` | SparkFun Pro Micro | `.hex` |
| `arduino-leonardo` | Arduino Leonardo | `.hex` |
| `arduino-micro` | Arduino Micro | `.hex` |
| `arduino-uno` | Arduino Uno | `.hex` |
| `arduino-nano` | Arduino Nano | `.hex` |
| `arduino-mega-2560` | Arduino Mega 2560 | `.hex` |
| `arduino-due` | Arduino Due | `.bin` |
| `esp32` | ESP32 DevKit | `.bin` |

## Usage

Applications can fetch the release manifest to:

1. Display a list of supported devices to the user
2. Download the appropriate firmware file based on user selection
3. Determine the correct flashing method based on file extension (`.hex` for AVR boards, `.bin` for ARM/ESP32 boards)
4. Configure upload parameters using the `uploadConfig` object:
   - Use the correct upload protocol for the device
   - Set the appropriate baud rate for flashing
   - Handle 1200bps touch requirement for native USB boards (ATmega32U4 and Arduino Due)
   - Identify the target MCU for proper flashing tool selection

### Fetching the Manifest

The manifest URL follows the pattern:
```
https://github.com/{owner}/{repo}/releases/download/{version}/release.json
```

To get the latest release manifest:
```
https://github.com/{owner}/{repo}/releases/latest/download/release.json
```
