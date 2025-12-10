#include "config_manager.h"
#include "device_info.h"

namespace ConfigManager {

// Global state
ConfigState g_config_state;
uint32_t g_current_config_id = 0;
static InputConfig g_current_inputs[MAX_INPUTS];
static uint8_t g_current_num_inputs = 0;

void init()
{
    // Try to load configuration from EEPROM
    if (loadFromEEPROM()) {
        // Configuration loaded successfully
    } else {
        // No valid configuration in EEPROM
        g_current_config_id = 0;
        g_current_num_inputs = 0;
    }
}

bool handleConfigure(const Protocol::Configure& cfg, bool& complete, bool& error)
{
    complete = false;
    error = false;

    // Check if this is a new configuration or continuation of existing one
    if (!g_config_state.isActive()) {
        // Start new configuration
        if (cfg.total_parts == 0 || cfg.total_parts > MAX_INPUTS) {
            error = true;
            return true; // Invalid total_parts
        }
        g_config_state.start(cfg.config_id, cfg.total_parts);
    } else {
        // Check if this is for the same configuration
        if (g_config_state.getConfigId() != cfg.config_id) {
            // Different config_id - discard old configuration and start new one
            g_config_state.reset();
            if (cfg.total_parts == 0 || cfg.total_parts > MAX_INPUTS) {
                error = true;
                return true;
            }
            g_config_state.start(cfg.config_id, cfg.total_parts);
        }
    }

    // Add this part to the configuration
    if (!g_config_state.addPart(cfg.part_number, cfg.input_type, cfg.pin, cfg.sensitivity)) {
        error = true;
        g_config_state.reset();
        return true;
    }

    // Check if configuration is complete
    if (g_config_state.isComplete()) {
        // Store to EEPROM
        storeToEEPROM(
            g_config_state.getConfigId(),
            g_config_state.getInputs(),
            g_config_state.getNumInputs());

        // Update current configuration
        g_current_config_id = g_config_state.getConfigId();
        g_current_num_inputs = g_config_state.getNumInputs();
        for (uint8_t i = 0; i < g_current_num_inputs; i++) {
            g_current_inputs[i] = g_config_state.getInputs()[i];
        }

        // Reset state
        g_config_state.reset();

        complete = true;
        return true;
    }

    return false; // Configuration in progress
}

bool checkTimeout()
{
    if (g_config_state.isActive() && g_config_state.hasTimedOut()) {
        g_config_state.reset();
        return true;
    }
    return false;
}

void storeToEEPROM(uint32_t config_id, const InputConfig* inputs, uint8_t num_inputs)
{
    int addr = 0;

    // Write magic number
    EEPROM.put(EEPROM_MAGIC_ADDR, EEPROM_MAGIC);

    // Write EEPROM format version (for compatibility checking)
    EEPROM.put(EEPROM_VERSION_ADDR, EEPROM_FORMAT_VERSION);

    // Write config_id
    EEPROM.put(EEPROM_CONFIG_ID_ADDR, config_id);

    // Write number of inputs
    EEPROM.put(EEPROM_NUM_INPUTS_ADDR, num_inputs);

    // Write input configurations
    addr = EEPROM_INPUTS_ADDR;
    for (uint8_t i = 0; i < num_inputs; i++) {
        EEPROM.put(addr, inputs[i].input_type);
        addr += sizeof(uint8_t);
        EEPROM.put(addr, inputs[i].pin);
        addr += sizeof(uint8_t);
        EEPROM.put(addr, inputs[i].sensitivity);
        addr += sizeof(uint8_t);
    }
}

bool loadFromEEPROM()
{
    // Read and verify magic number
    uint32_t magic;
    EEPROM.get(EEPROM_MAGIC_ADDR, magic);
    if (magic != EEPROM_MAGIC) {
        return false; // No valid configuration
    }

    // Read and verify EEPROM format version
    uint8_t stored_version;
    EEPROM.get(EEPROM_VERSION_ADDR, stored_version);
    if (stored_version != EEPROM_FORMAT_VERSION) {
        // Version mismatch - clear EEPROM to invalidate old config
        // This prevents loading incompatible configurations after firmware updates
        uint32_t zero = 0;
        EEPROM.put(EEPROM_MAGIC_ADDR, zero);
        return false;
    }

    // Read config_id
    EEPROM.get(EEPROM_CONFIG_ID_ADDR, g_current_config_id);

    // Read number of inputs
    EEPROM.get(EEPROM_NUM_INPUTS_ADDR, g_current_num_inputs);

    // Validate number of inputs
    if (g_current_num_inputs == 0 || g_current_num_inputs > MAX_INPUTS) {
        return false;
    }

    // Read input configurations
    int addr = EEPROM_INPUTS_ADDR;
    for (uint8_t i = 0; i < g_current_num_inputs; i++) {
        EEPROM.get(addr, g_current_inputs[i].input_type);
        addr += sizeof(uint8_t);
        EEPROM.get(addr, g_current_inputs[i].pin);
        addr += sizeof(uint8_t);
        EEPROM.get(addr, g_current_inputs[i].sensitivity);
        addr += sizeof(uint8_t);
    }

    return true;
}

uint32_t getCurrentConfigId()
{
    return g_current_config_id;
}

const InputConfig* getCurrentConfig(uint8_t& num_inputs)
{
    num_inputs = g_current_num_inputs;
    return g_current_inputs;
}

} // namespace ConfigManager
