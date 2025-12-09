#pragma once

#include <stdint.h>

namespace Heartbeat {

/**
 * Callback function type for sending heartbeat
 */
typedef void (*HeartbeatCallback)();

/**
 * Heartbeat manager - sends periodic heartbeat messages only when no other
 * messages have been sent recently.
 *
 * This keeps the connection alive during idle periods without adding
 * unnecessary traffic when the device is actively sending data.
 */
class HeartbeatManager {
public:
    /**
     * Constructor
     * @param interval_ms Heartbeat interval in milliseconds
     * @param callback Function to call when heartbeat should be sent (optional)
     */
    explicit HeartbeatManager(unsigned long interval_ms, HeartbeatCallback callback = nullptr);

    /**
     * Set the callback function for sending heartbeat
     * @param callback Function to call when heartbeat should be sent
     */
    void setCallback(HeartbeatCallback callback);

    /**
     * Update the heartbeat manager - call this in your main loop
     * This will automatically send heartbeat if needed
     * @param timestamp Current time in milliseconds (from millis())
     */
    void update(unsigned long timestamp);

    /**
     * Notify that a message was sent
     * This resets the heartbeat timer
     * @param timestamp Current time in milliseconds (from millis())
     */
    void notifyMessageSent(unsigned long timestamp);

    /**
     * Check if a heartbeat should be sent
     * @param timestamp Current time in milliseconds (from millis())
     * @return true if a heartbeat should be sent
     */
    bool shouldSendHeartbeat(unsigned long timestamp) const;

    /**
     * Mark that a heartbeat was sent
     * This updates the last message time
     * @param timestamp Current time in milliseconds (from millis())
     */
    void markHeartbeatSent(unsigned long timestamp);

    /**
     * Get the configured heartbeat interval
     * @return Heartbeat interval in milliseconds
     */
    unsigned long getInterval() const { return m_interval_ms; }

    /**
     * Get the time of the last message sent
     * @return Timestamp of last message in milliseconds
     */
    unsigned long getLastMessageTime() const { return m_last_message_time; }

private:
    unsigned long m_interval_ms;
    unsigned long m_last_message_time;
    HeartbeatCallback m_callback;
};

} // namespace Heartbeat
