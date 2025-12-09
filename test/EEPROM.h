// Mock EEPROM.h for native testing
#pragma once

#include <stdint.h>
#include <string.h>

// Mock EEPROM storage
extern uint8_t mock_eeprom_storage[1024];

class EEPROMClass {
public:
    template <typename T>
    T& get(int idx, T& t) {
        uint8_t* ptr = (uint8_t*)&t;
        for (size_t i = 0; i < sizeof(T); i++) {
            *ptr++ = mock_eeprom_storage[idx++];
        }
        return t;
    }

    template <typename T>
    const T& put(int idx, const T& t) {
        const uint8_t* ptr = (const uint8_t*)&t;
        for (size_t i = 0; i < sizeof(T); i++) {
            mock_eeprom_storage[idx++] = *ptr++;
        }
        return t;
    }

    uint8_t read(int idx) {
        return mock_eeprom_storage[idx];
    }

    void write(int idx, uint8_t val) {
        mock_eeprom_storage[idx] = val;
    }

    void update(int idx, uint8_t val) {
        mock_eeprom_storage[idx] = val;
    }

    uint16_t length() {
        return 1024;
    }
};

extern EEPROMClass EEPROM;

