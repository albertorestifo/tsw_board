# TWS-IO Device Firmware

Arduino firmware for the TWS-IO device.

## Architecture

The firmware is organized into modular components for maintainability and testability.

### Module Organization

```
board/src/
├── main.cpp              # Application entry point (minimal)
├── protocol.h/cpp        # Protocol message definitions
├── message_handler.h/cpp # Message handling and communication
├── config_manager.h/cpp  # Configuration management
├── sensor_manager.h/cpp  # Sensor lifecycle management
├── sensor.h              # Sensor interface
└── analog_sensor.h/cpp   # Analog sensor implementation
```

### Module Responsibilities

#### `main.cpp` - Application Entry Point

**Responsibility:** Minimal coordination and initialization

- Initializes serial communication
- Initializes subsystems (ConfigManager, MessageHandler)
- Main event loop (update packet serial and message handler)
- Delegates all logic to specialized modules

**Size:** ~36 lines

#### `protocol.h/cpp` - Protocol Definitions

**Responsibility:** Protocol message encoding/decoding

- Message type constants (MESSAGE*TYPE*\*)
- Message structs:
  - `IdentityRequest` - Request device identity
  - `IdentityResponse` - Device identity response
  - `Configure` - Configuration message
  - `ConfigurationStored` - Configuration success response
  - `ConfigurationError` - Configuration error response
- `encode()` and `decode()` methods for each message type
- Generic `Message` union for decoding any message type

**Key Features:**

- Each message type is self-contained
- Scalable design - adding new messages is straightforward
- Little-endian encoding for multi-byte integers
- Comprehensive validation

#### `message_handler.h/cpp` - Message Handling

**Responsibility:** Message routing and communication

- Manages PacketSerial instance
- Routes incoming messages to appropriate handlers
- Implements message-specific handling logic:
  - `handleIdentityRequest()` - Responds with device identity
  - `handleConfigure()` - Delegates to ConfigManager
- Sends outgoing messages:
  - `sendIdentityResponse()`
  - `sendConfigurationStored()`
  - `sendConfigurationError()`
- Handles periodic tasks (timeout checking)

**Key Features:**

- Centralized message routing
- Encapsulates all serial communication
- Clean separation between message handling and business logic

#### `config_manager.h/cpp` - Configuration Management

**Responsibility:** Device configuration lifecycle

- Configuration accumulation in RAM
- Part tracking (which parts received)
- Timeout handling (5-second timeout)
- EEPROM storage and loading
- Configuration validation

**Key Components:**

- `ConfigState` - Tracks active configuration accumulation
- `InputConfig` - Single input configuration
- `init()` - Loads configuration from EEPROM on startup
- `handleConfigure()` - Processes Configure messages
- `checkTimeout()` - Checks for configuration timeout
- `storeToEEPROM()` / `loadFromEEPROM()` - Persistence

**Key Features:**

- Supports up to 8 inputs (MAX_INPUTS)
- Validates configuration completeness
- Persists configuration across power cycles
- Handles incomplete/failed configurations gracefully

#### `sensor_manager.h/cpp` - Sensor Lifecycle Management

**Responsibility:** Manage sensor instances and coordinate scanning

- Creates sensor instances based on configuration
- Manages sensor lifecycle (creation, initialization, destruction)
- Coordinates sensor scanning across all active sensors
- Collects readings from sensors (round-robin)
- Provides abstraction layer between sensors and message handler

**Key Components:**

- `init()` - Initialize sensor manager
- `applyConfiguration()` - Create sensors from configuration
- `scan()` - Scan all active sensors
- `getNextReading()` - Get next available sensor reading
- `getSensorCount()` - Get number of active sensors

**Key Features:**

- Supports up to 8 sensors (MAX_SENSORS)
- Dynamic sensor creation based on configuration
- Round-robin reading retrieval for fairness
- Automatic sensor cleanup on reconfiguration

#### `sensor.h` - Sensor Interface

**Responsibility:** Define common sensor interface

- `ISensor` interface - Base class for all sensors
- `Reading` struct - Sensor reading result
- `InputType` enum - Sensor type enumeration

**Interface Methods:**

- `begin()` - Initialize sensor hardware
- `scan()` - Read current value, update running average
- `getReading()` - Check if value ready to report
- `getType()` - Get sensor input type
- `getPin()` - Get sensor pin number

#### `analog_sensor.h/cpp` - Analog Sensor Implementation

**Responsibility:** Analog input sensor implementation

- Reads analog input using Arduino ADC
- Maintains running average of readings
- Reports based on sensitivity setting
- Normalizes values to int32 range

**Key Features:**

- 10-bit ADC reading (0-1023)
- Running average accumulation
- Sensitivity-based reporting (0-255)
  - 0 = report every 255 scans
  - 255 = report every scan
- Value normalization (shift left 22 bits)
- Configurable pin and sensitivity

## Data Flow

### Startup Sequence

```
1. main.cpp::setup()
   ├─> g_packet_serial.begin(115200)
   ├─> ConfigManager::init()
   │   └─> Load configuration from EEPROM
   ├─> SensorManager::init()
   ├─> SensorManager::applyConfiguration()
   │   ├─> Create sensor instances from loaded config
   │   └─> Initialize each sensor (begin())
   └─> MessageHandler::init(&g_packet_serial)

2. main.cpp::loop()
   ├─> g_packet_serial.update()
   │   └─> Calls onPacketReceived() for each packet
   └─> MessageHandler::update()
       ├─> Check for configuration timeout
       ├─> SensorManager::scan()
       │   └─> Scan all sensors (read values, accumulate)
       └─> SensorManager::getNextReading()
           └─> Send InputValue messages for ready sensors
```

### Message Handling Flow

```
1. Packet arrives via serial
   └─> PacketSerial calls onPacketReceived()

2. main.cpp::onPacketReceived()
   └─> MessageHandler::onPacketReceived()

3. MessageHandler::onPacketReceived()
   ├─> Decode message using Protocol::Message
   └─> Route to appropriate handler:
       ├─> handleIdentityRequest()
       │   └─> sendIdentityResponse()
       └─> handleConfigure()
           ├─> ConfigManager::handleConfigure()
           └─> Send response:
               ├─> sendConfigurationStored() (on success)
               └─> sendConfigurationError() (on error)
```

### Configuration Flow

```
1. Host sends Configure messages (one per input)
   └─> MessageHandler::handleConfigure()

2. ConfigManager::handleConfigure()
   ├─> Start new configuration (if first part)
   ├─> Add part to accumulation
   ├─> Check if complete
   └─> Return status (complete/error/in-progress)

3. On completion:
   ├─> ConfigManager::storeToEEPROM()
   ├─> SensorManager::applyConfiguration()
   │   ├─> Destroy old sensors
   │   ├─> Create new sensors from config
   │   └─> Initialize each sensor
   └─> MessageHandler::sendConfigurationStored()

4. On timeout (in main loop):
   ├─> ConfigManager::checkTimeout()
   └─> MessageHandler::sendConfigurationError()
```

### Sensor Scanning Flow

```
1. Every loop iteration:
   └─> MessageHandler::update()
       └─> SensorManager::scan()
           └─> For each sensor:
               ├─> Read current value (e.g., analogRead)
               ├─> Accumulate in running average
               └─> Increment scan counter

2. Check for readings:
   └─> SensorManager::getNextReading()
       └─> For each sensor (round-robin):
           ├─> Check if scans_since_report >= scans_per_report
           ├─> If ready:
           │   ├─> Calculate average of accumulated samples
           │   ├─> Normalize value to int32
           │   ├─> Return Reading{has_value=true, value, type}
           │   └─> Reset accumulator
           └─> If not ready:
               └─> Return Reading{has_value=false}

3. Send readings:
   └─> MessageHandler::sendInputValue()
       ├─> Create InputValue message
       ├─> Encode message
       └─> Send via PacketSerial
```

## Testing

The protocol module has comprehensive unit tests that run on the native platform:

```bash
pio test -e native
```

Tests cover:

- Message encoding/decoding for all message types
- Message roundtrip (encode → decode → verify)
- Error handling (buffer too small, insufficient data, invalid type)
- Message union decoding

## Adding New Message Types

To add a new message type:

1. **Add message type constant** in `protocol.h`:

   ```cpp
   constexpr uint8_t MESSAGE_TYPE_NEW_MESSAGE = 5;
   ```

2. **Define message struct** in `protocol.h`:

   ```cpp
   struct NewMessage {
       uint32_t field1;
       uint8_t field2;

       size_t encode(uint8_t* buffer, size_t buffer_size) const;
       bool decode(const uint8_t* buffer, size_t length);
   };
   ```

3. **Implement encode/decode** in `protocol.cpp`

4. **Add to Message union** in `protocol.h`:

   ```cpp
   struct Message {
       union {
           // ... existing messages
           NewMessage new_message;
       };
       bool isNewMessage() const { return message_type == MESSAGE_TYPE_NEW_MESSAGE; }
   };
   ```

5. **Update Message::decode()** in `protocol.cpp`

6. **Add handler** in `message_handler.h/cpp`:

   ```cpp
   void handleNewMessage(const Protocol::NewMessage& msg);
   ```

7. **Route message** in `MessageHandler::onPacketReceived()`

8. **Add tests** in `test/test_protocol.cpp`

## Dependencies

- **Arduino Framework** - Core Arduino functionality
- **PacketSerial** (v1.4.0) - COBS framing for reliable serial communication
- **EEPROM** - Configuration persistence

## Build Environments

- **sparkfun_promicro16** - Target hardware (Arduino Pro Micro)
- **native** - Unit testing environment
