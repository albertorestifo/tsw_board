# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.1] - 2025-12-10

### Changed

- **BREAKING**: `IdentityResponse` protocol message format changed
  - Version field changed from single `u8` to three separate fields: `version_major`, `version_minor`, `version_patch` (all `u8`)
  - Removed `device_id` field entirely
  - Message size changed from 11 bytes to 12 bytes

### Protocol Migration

**Old format (v1.0.0):**
```
[type: u8 = 1] [request_id: u32] [version: u8] [device_id: u8] [config_id: u32]
```

**New format (v1.0.1):**
```
[type: u8 = 1] [request_id: u32] [version_major: u8] [version_minor: u8] [version_patch: u8] [config_id: u32]
```

Hosts communicating with devices running this firmware version must update their protocol parsers accordingly.

## [1.0.0] - 2025-12-10

### Added

- Initial release
- Binary protocol over serial using COBS framing
- Message types: IdentityRequest, IdentityResponse, Configure, ConfigurationStored, ConfigurationError, InputValue, Heartbeat
- Analog sensor input support with configurable sensitivity
- EEPROM configuration persistence
- Heartbeat keep-alive mechanism
