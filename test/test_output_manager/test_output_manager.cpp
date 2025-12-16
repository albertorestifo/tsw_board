// Mock Arduino environment for native testing
#include <stdint.h>
#include <string.h>

// Arduino pin definitions
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define LOW 0
#define HIGH 1

// Mock state tracking
static uint8_t g_pin_modes[32];
static uint8_t g_pin_values[32];
static uint8_t g_pinMode_call_count = 0;
static uint8_t g_digitalWrite_call_count = 0;

void pinMode(uint8_t pin, uint8_t mode)
{
    if (pin < 32) {
        g_pin_modes[pin] = mode;
    }
    g_pinMode_call_count++;
}

void digitalWrite(uint8_t pin, uint8_t val)
{
    if (pin < 32) {
        g_pin_values[pin] = val;
    }
    g_digitalWrite_call_count++;
}

// Now include the output manager code
#include "../../src/output_manager.cpp"
#include <unity.h>

// Helper to reset mock state
void resetMockState()
{
    memset(g_pin_modes, 0xFF, sizeof(g_pin_modes)); // 0xFF = unset
    memset(g_pin_values, 0xFF, sizeof(g_pin_values));
    g_pinMode_call_count = 0;
    g_digitalWrite_call_count = 0;
    OutputManager::init();
}

void setUp()
{
    resetMockState();
}

void tearDown()
{
    // Nothing to clean up
}

// Test that init resets state
void test_output_manager_init()
{
    // Set an output first
    OutputManager::setOutput(5, 1);
    TEST_ASSERT_EQUAL_UINT8(OUTPUT, g_pin_modes[5]);

    // Reset and verify pin tracking is cleared
    resetMockState();
    g_pinMode_call_count = 0;

    // Setting same pin should call pinMode again after init
    OutputManager::setOutput(5, 1);
    TEST_ASSERT_EQUAL_UINT8(1, g_pinMode_call_count);
}

// Test that setOutput configures pin as OUTPUT on first use
void test_output_manager_configures_pin_on_first_use()
{
    OutputManager::setOutput(13, 1);

    TEST_ASSERT_EQUAL_UINT8(OUTPUT, g_pin_modes[13]);
    TEST_ASSERT_EQUAL_UINT8(1, g_pinMode_call_count);
}

// Test that setOutput sets correct value
void test_output_manager_sets_value()
{
    OutputManager::setOutput(5, 1);
    TEST_ASSERT_EQUAL_UINT8(HIGH, g_pin_values[5]);

    OutputManager::setOutput(6, 0);
    TEST_ASSERT_EQUAL_UINT8(LOW, g_pin_values[6]);
}

// Test that repeated calls don't reconfigure pinMode
void test_output_manager_no_repeated_pinMode()
{
    OutputManager::setOutput(9, 1);
    TEST_ASSERT_EQUAL_UINT8(1, g_pinMode_call_count);

    OutputManager::setOutput(9, 0);
    TEST_ASSERT_EQUAL_UINT8(1, g_pinMode_call_count); // Still 1, not 2

    OutputManager::setOutput(9, 1);
    TEST_ASSERT_EQUAL_UINT8(1, g_pinMode_call_count); // Still 1
}

// Test that digitalWrite is called every time
void test_output_manager_always_calls_digitalWrite()
{
    OutputManager::setOutput(7, 1);
    TEST_ASSERT_EQUAL_UINT8(1, g_digitalWrite_call_count);

    OutputManager::setOutput(7, 0);
    TEST_ASSERT_EQUAL_UINT8(2, g_digitalWrite_call_count);

    OutputManager::setOutput(7, 1);
    TEST_ASSERT_EQUAL_UINT8(3, g_digitalWrite_call_count);
}

// Test multiple different pins
void test_output_manager_multiple_pins()
{
    OutputManager::setOutput(2, 1);
    OutputManager::setOutput(3, 0);
    OutputManager::setOutput(4, 1);

    TEST_ASSERT_EQUAL_UINT8(3, g_pinMode_call_count);
    TEST_ASSERT_EQUAL_UINT8(HIGH, g_pin_values[2]);
    TEST_ASSERT_EQUAL_UINT8(LOW, g_pin_values[3]);
    TEST_ASSERT_EQUAL_UINT8(HIGH, g_pin_values[4]);
}

// Test non-zero value maps to HIGH
void test_output_manager_nonzero_is_high()
{
    OutputManager::setOutput(10, 255);
    TEST_ASSERT_EQUAL_UINT8(HIGH, g_pin_values[10]);

    OutputManager::setOutput(11, 42);
    TEST_ASSERT_EQUAL_UINT8(HIGH, g_pin_values[11]);
}

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_output_manager_init);
    RUN_TEST(test_output_manager_configures_pin_on_first_use);
    RUN_TEST(test_output_manager_sets_value);
    RUN_TEST(test_output_manager_no_repeated_pinMode);
    RUN_TEST(test_output_manager_always_calls_digitalWrite);
    RUN_TEST(test_output_manager_multiple_pins);
    RUN_TEST(test_output_manager_nonzero_is_high);

    return UNITY_END();
}
