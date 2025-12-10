#pragma once

#include <stdint.h>

// Device version (semantic versioning)
constexpr uint8_t DEVICE_VERSION_MAJOR = 1;
constexpr uint8_t DEVICE_VERSION_MINOR = 0;
constexpr uint8_t DEVICE_VERSION_PATCH = 1;

// EEPROM format version - increment when EEPROM layout changes
constexpr uint8_t EEPROM_FORMAT_VERSION = 1;
