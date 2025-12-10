#include "protocol.h"
#include <string.h>

namespace Protocol {

// IdentityRequest implementation

size_t IdentityRequest::encode(uint8_t* buffer, size_t buffer_size) const
{
    constexpr size_t REQUIRED_SIZE = 5; // 1 byte type + 4 bytes request_id

    if (buffer_size < REQUIRED_SIZE) {
        return 0; // Buffer too small
    }

    size_t offset = 0;

    // Message type (u8)
    buffer[offset++] = MESSAGE_TYPE_IDENTITY_REQUEST;

    // request_id (u32) - little endian
    buffer[offset++] = (request_id >> 0) & 0xFF;
    buffer[offset++] = (request_id >> 8) & 0xFF;
    buffer[offset++] = (request_id >> 16) & 0xFF;
    buffer[offset++] = (request_id >> 24) & 0xFF;

    return offset;
}

bool IdentityRequest::decode(const uint8_t* buffer, size_t length)
{
    constexpr size_t REQUIRED_SIZE = 5;

    if (length < REQUIRED_SIZE) {
        return false; // Not enough data
    }

    if (buffer[0] != MESSAGE_TYPE_IDENTITY_REQUEST) {
        return false; // Wrong message type
    }

    size_t offset = 1;

    // request_id (u32) - little endian
    request_id = ((uint32_t)buffer[offset + 0] << 0) | ((uint32_t)buffer[offset + 1] << 8) | ((uint32_t)buffer[offset + 2] << 16) | ((uint32_t)buffer[offset + 3] << 24);

    return true;
}

// IdentityResponse implementation

size_t IdentityResponse::encode(uint8_t* buffer, size_t buffer_size) const
{
    // 1 type + 4 request_id + 1 version_major + 1 version_minor + 1 version_patch + 4 config_id = 12
    constexpr size_t REQUIRED_SIZE = 12;

    if (buffer_size < REQUIRED_SIZE) {
        return 0; // Buffer too small
    }

    size_t offset = 0;

    // Message type (u8)
    buffer[offset++] = MESSAGE_TYPE_IDENTITY_RESPONSE;

    // request_id (u32) - little endian
    buffer[offset++] = (request_id >> 0) & 0xFF;
    buffer[offset++] = (request_id >> 8) & 0xFF;
    buffer[offset++] = (request_id >> 16) & 0xFF;
    buffer[offset++] = (request_id >> 24) & 0xFF;

    // version_major (u8)
    buffer[offset++] = version_major;

    // version_minor (u8)
    buffer[offset++] = version_minor;

    // version_patch (u8)
    buffer[offset++] = version_patch;

    // config_id (u32) - little endian
    buffer[offset++] = (config_id >> 0) & 0xFF;
    buffer[offset++] = (config_id >> 8) & 0xFF;
    buffer[offset++] = (config_id >> 16) & 0xFF;
    buffer[offset++] = (config_id >> 24) & 0xFF;

    return offset;
}

bool IdentityResponse::decode(const uint8_t* buffer, size_t length)
{
    constexpr size_t REQUIRED_SIZE = 12;

    if (length < REQUIRED_SIZE) {
        return false; // Not enough data
    }

    if (buffer[0] != MESSAGE_TYPE_IDENTITY_RESPONSE) {
        return false; // Wrong message type
    }

    size_t offset = 1;

    // request_id (u32) - little endian
    request_id = ((uint32_t)buffer[offset + 0] << 0) | ((uint32_t)buffer[offset + 1] << 8) | ((uint32_t)buffer[offset + 2] << 16) | ((uint32_t)buffer[offset + 3] << 24);
    offset += 4;

    // version_major (u8)
    version_major = buffer[offset++];

    // version_minor (u8)
    version_minor = buffer[offset++];

    // version_patch (u8)
    version_patch = buffer[offset++];

    // config_id (u32) - little endian
    config_id = ((uint32_t)buffer[offset + 0] << 0) | ((uint32_t)buffer[offset + 1] << 8) | ((uint32_t)buffer[offset + 2] << 16) | ((uint32_t)buffer[offset + 3] << 24);

    return true;
}

// Configure implementation

size_t Configure::encode(uint8_t* buffer, size_t buffer_size) const
{
    constexpr size_t REQUIRED_SIZE = 10; // 1 type + 4 config_id + 1 total_parts + 1 part_number + 1 input_type + 1 pin + 1 sensitivity

    if (buffer_size < REQUIRED_SIZE) {
        return 0; // Buffer too small
    }

    size_t offset = 0;

    // Message type (u8)
    buffer[offset++] = MESSAGE_TYPE_CONFIGURE;

    // config_id (u32) - little endian
    buffer[offset++] = (config_id >> 0) & 0xFF;
    buffer[offset++] = (config_id >> 8) & 0xFF;
    buffer[offset++] = (config_id >> 16) & 0xFF;
    buffer[offset++] = (config_id >> 24) & 0xFF;

    // total_parts (u8)
    buffer[offset++] = total_parts;

    // part_number (u8)
    buffer[offset++] = part_number;

    // input_type (u8)
    buffer[offset++] = input_type;

    // pin (u8)
    buffer[offset++] = pin;

    // sensitivity (u8)
    buffer[offset++] = sensitivity;

    return offset;
}

bool Configure::decode(const uint8_t* buffer, size_t length)
{
    constexpr size_t REQUIRED_SIZE = 10;

    if (length < REQUIRED_SIZE) {
        return false; // Not enough data
    }

    if (buffer[0] != MESSAGE_TYPE_CONFIGURE) {
        return false; // Wrong message type
    }

    size_t offset = 1;

    // config_id (u32) - little endian
    config_id = ((uint32_t)buffer[offset + 0] << 0) | ((uint32_t)buffer[offset + 1] << 8) | ((uint32_t)buffer[offset + 2] << 16) | ((uint32_t)buffer[offset + 3] << 24);
    offset += 4;

    // total_parts (u8)
    total_parts = buffer[offset++];

    // part_number (u8)
    part_number = buffer[offset++];

    // input_type (u8)
    input_type = buffer[offset++];

    // pin (u8)
    pin = buffer[offset++];

    // sensitivity (u8)
    sensitivity = buffer[offset++];

    return true;
}

// ConfigurationStored implementation

size_t ConfigurationStored::encode(uint8_t* buffer, size_t buffer_size) const
{
    constexpr size_t REQUIRED_SIZE = 5; // 1 type + 4 config_id

    if (buffer_size < REQUIRED_SIZE) {
        return 0; // Buffer too small
    }

    size_t offset = 0;

    // Message type (u8)
    buffer[offset++] = MESSAGE_TYPE_CONFIGURATION_STORED;

    // config_id (u32) - little endian
    buffer[offset++] = (config_id >> 0) & 0xFF;
    buffer[offset++] = (config_id >> 8) & 0xFF;
    buffer[offset++] = (config_id >> 16) & 0xFF;
    buffer[offset++] = (config_id >> 24) & 0xFF;

    return offset;
}

bool ConfigurationStored::decode(const uint8_t* buffer, size_t length)
{
    constexpr size_t REQUIRED_SIZE = 5;

    if (length < REQUIRED_SIZE) {
        return false; // Not enough data
    }

    if (buffer[0] != MESSAGE_TYPE_CONFIGURATION_STORED) {
        return false; // Wrong message type
    }

    size_t offset = 1;

    // config_id (u32) - little endian
    config_id = ((uint32_t)buffer[offset + 0] << 0) | ((uint32_t)buffer[offset + 1] << 8) | ((uint32_t)buffer[offset + 2] << 16) | ((uint32_t)buffer[offset + 3] << 24);

    return true;
}

// ConfigurationError implementation

size_t ConfigurationError::encode(uint8_t* buffer, size_t buffer_size) const
{
    constexpr size_t REQUIRED_SIZE = 5; // 1 type + 4 config_id

    if (buffer_size < REQUIRED_SIZE) {
        return 0; // Buffer too small
    }

    size_t offset = 0;

    // Message type (u8)
    buffer[offset++] = MESSAGE_TYPE_CONFIGURATION_ERROR;

    // config_id (u32) - little endian
    buffer[offset++] = (config_id >> 0) & 0xFF;
    buffer[offset++] = (config_id >> 8) & 0xFF;
    buffer[offset++] = (config_id >> 16) & 0xFF;
    buffer[offset++] = (config_id >> 24) & 0xFF;

    return offset;
}

bool ConfigurationError::decode(const uint8_t* buffer, size_t length)
{
    constexpr size_t REQUIRED_SIZE = 5;

    if (length < REQUIRED_SIZE) {
        return false; // Not enough data
    }

    if (buffer[0] != MESSAGE_TYPE_CONFIGURATION_ERROR) {
        return false; // Wrong message type
    }

    size_t offset = 1;

    // config_id (u32) - little endian
    config_id = ((uint32_t)buffer[offset + 0] << 0) | ((uint32_t)buffer[offset + 1] << 8) | ((uint32_t)buffer[offset + 2] << 16) | ((uint32_t)buffer[offset + 3] << 24);

    return true;
}

// InputValue implementation

size_t InputValue::encode(uint8_t* buffer, size_t buffer_size) const
{
    constexpr size_t REQUIRED_SIZE = 4;

    if (buffer_size < REQUIRED_SIZE) {
        return 0; // Buffer too small
    }

    size_t offset = 0;

    // Message type (u8)
    buffer[offset++] = MESSAGE_TYPE_INPUT_VALUE;

    // pin (u8)
    buffer[offset++] = pin;

    // value (i16) - little endian
    buffer[offset++] = (value >> 0) & 0xFF;
    buffer[offset++] = (value >> 8) & 0xFF;

    return offset;
}

bool InputValue::decode(const uint8_t* buffer, size_t length)
{
    constexpr size_t REQUIRED_SIZE = 4;

    if (length < REQUIRED_SIZE) {
        return false; // Not enough data
    }

    if (buffer[0] != MESSAGE_TYPE_INPUT_VALUE) {
        return false; // Wrong message type
    }

    size_t offset = 1;

    // pin (u8)
    pin = buffer[offset++];

    // value (i16) - little endian
    value = (int16_t)(((uint16_t)buffer[offset + 0] << 0) | ((uint16_t)buffer[offset + 1] << 8));

    return true;
}

// Heartbeat implementation

size_t Heartbeat::encode(uint8_t* buffer, size_t buffer_size) const
{
    constexpr size_t REQUIRED_SIZE = 1; // Just the message type

    if (buffer_size < REQUIRED_SIZE) {
        return 0; // Buffer too small
    }

    buffer[0] = MESSAGE_TYPE_HEARTBEAT;

    return REQUIRED_SIZE;
}

bool Heartbeat::decode(const uint8_t* buffer, size_t length)
{
    constexpr size_t REQUIRED_SIZE = 1;

    if (length < REQUIRED_SIZE) {
        return false; // Not enough data
    }

    if (buffer[0] != MESSAGE_TYPE_HEARTBEAT) {
        return false; // Wrong message type
    }

    return true;
}

// Message implementation (for generic decoding)

bool Message::decode(const uint8_t* buffer, size_t length)
{
    if (length < 1) {
        return false; // Need at least message type
    }

    message_type = buffer[0];

    switch (message_type) {
    case MESSAGE_TYPE_IDENTITY_REQUEST:
        return identity_request.decode(buffer, length);

    case MESSAGE_TYPE_IDENTITY_RESPONSE:
        return identity_response.decode(buffer, length);

    case MESSAGE_TYPE_CONFIGURE:
        return configure.decode(buffer, length);

    case MESSAGE_TYPE_CONFIGURATION_STORED:
        return configuration_stored.decode(buffer, length);

    case MESSAGE_TYPE_CONFIGURATION_ERROR:
        return configuration_error.decode(buffer, length);

    case MESSAGE_TYPE_INPUT_VALUE:
        return input_value.decode(buffer, length);

    case MESSAGE_TYPE_HEARTBEAT:
        return heartbeat.decode(buffer, length);

    default:
        return false; // Unknown message type
    }
}

} // namespace Protocol
