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
      "firmwareFile": "string"
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
      "firmwareFile": "trenino-sparkfun-pro-micro.firmware.hex"
    },
    {
      "environment": "leonardo",
      "name": "arduino-leonardo",
      "displayName": "Arduino Leonardo",
      "firmwareFile": "trenino-arduino-leonardo.firmware.hex"
    },
    {
      "environment": "esp32dev",
      "name": "esp32",
      "displayName": "ESP32 DevKit",
      "firmwareFile": "trenino-esp32.firmware.bin"
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

### Fetching the Manifest

The manifest URL follows the pattern:
```
https://github.com/{owner}/{repo}/releases/download/{version}/release.json
```

To get the latest release manifest:
```
https://github.com/{owner}/{repo}/releases/latest/download/release.json
```
