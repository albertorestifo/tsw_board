#pragma once

#include "protocol.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <stdint.h>

namespace ConfigManager {

// Maximum number of inputs that can be configured
constexpr uint8_t MAX_INPUTS = 8;

// Timeout for configuration in milliseconds (5 seconds)
constexpr unsigned long CONFIG_TIMEOUT_MS = 5000;

// EEPROM addresses
constexpr int EEPROM_MAGIC_ADDR = 0; // 4 bytes - magic number to validate EEPROM
constexpr int EEPROM_VERSION_ADDR = 4; // 1 byte - device version that created this config
constexpr int EEPROM_CONFIG_ID_ADDR = 5; // 4 bytes - config_id
constexpr int EEPROM_NUM_INPUTS_ADDR = 9; // 1 byte - number of inputs
constexpr int EEPROM_INPUTS_ADDR = 10; // Start of input configurations

// Magic number to validate EEPROM data
constexpr uint32_t EEPROM_MAGIC = 0xC0FF1234;

// Single input configuration
struct InputConfig {
    uint8_t input_type;
    uint8_t pin;
    uint8_t sensitivity;

    InputConfig()
        : input_type(0)
        , pin(0)
        , sensitivity(0)
    {
    }
};

// Configuration state
class ConfigState {
private:
    uint32_t config_id;
    uint8_t total_parts;
    uint8_t received_parts;
    bool parts_received[MAX_INPUTS]; // Track which parts we've received
    InputConfig inputs[MAX_INPUTS];
    unsigned long start_time; // When we started receiving this configuration
    bool active; // Is there an active configuration being received?

public:
    ConfigState()
        : config_id(0)
        , total_parts(0)
        , received_parts(0)
        , start_time(0)
        , active(false)
    {
        for (uint8_t i = 0; i < MAX_INPUTS; i++) {
            parts_received[i] = false;
        }
    }

    // Start a new configuration
    void start(uint32_t cfg_id, uint8_t total)
    {
        config_id = cfg_id;
        total_parts = total;
        received_parts = 0;
        start_time = millis();
        active = true;

        for (uint8_t i = 0; i < MAX_INPUTS; i++) {
            parts_received[i] = false;
            inputs[i] = InputConfig();
        }
    }

    // Add a configuration part
    bool addPart(uint8_t part_number, uint8_t input_type, uint8_t pin, uint8_t sensitivity)
    {
        if (!active || part_number >= total_parts || part_number >= MAX_INPUTS) {
            return false;
        }

        // Store the input configuration
        inputs[part_number].input_type = input_type;
        inputs[part_number].pin = pin;
        inputs[part_number].sensitivity = sensitivity;

        // Mark this part as received
        if (!parts_received[part_number]) {
            parts_received[part_number] = true;
            received_parts++;
        }

        return true;
    }

    // Check if configuration is complete
    bool isComplete() const
    {
        return active && received_parts == total_parts;
    }

    // Check if configuration has timed out
    bool hasTimedOut() const
    {
        return active && (millis() - start_time) > CONFIG_TIMEOUT_MS;
    }

    // Get the configuration ID
    uint32_t getConfigId() const
    {
        return config_id;
    }

    // Get the inputs
    const InputConfig* getInputs() const
    {
        return inputs;
    }

    // Get the number of inputs
    uint8_t getNumInputs() const
    {
        return total_parts;
    }

    // Reset the state
    void reset()
    {
        active = false;
        config_id = 0;
        total_parts = 0;
        received_parts = 0;
    }

    // Check if there's an active configuration
    bool isActive() const
    {
        return active;
    }
};

// Global configuration state
extern ConfigState g_config_state;
extern uint32_t g_current_config_id;

// Initialize configuration manager
void init();

// Handle a Configure message
// Returns true if configuration is complete or error occurred
// Sets complete=true if configuration is complete
// Sets error=true if configuration failed
bool handleConfigure(const Protocol::Configure& cfg, bool& complete, bool& error);

// Check for configuration timeout
// Returns true if timeout occurred
bool checkTimeout();

// Store configuration to EEPROM
void storeToEEPROM(uint32_t config_id, const InputConfig* inputs, uint8_t num_inputs);

// Load configuration from EEPROM
// Returns true if valid configuration was loaded
bool loadFromEEPROM();

// Get current configuration ID
uint32_t getCurrentConfigId();

// Get current configuration
const InputConfig* getCurrentConfig(uint8_t& num_inputs);

} // namespace ConfigManager
