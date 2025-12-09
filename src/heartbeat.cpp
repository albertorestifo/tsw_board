#include "heartbeat.h"

namespace Heartbeat {

HeartbeatManager::HeartbeatManager(unsigned long interval_ms, HeartbeatCallback callback)
    : m_interval_ms(interval_ms)
    , m_last_message_time(0)
    , m_callback(callback)
{
}

void HeartbeatManager::setCallback(HeartbeatCallback callback)
{
    m_callback = callback;
}

void HeartbeatManager::update(unsigned long timestamp)
{
    // Check if heartbeat should be sent
    if (shouldSendHeartbeat(timestamp)) {
        // Call the callback if set
        if (m_callback) {
            m_callback();
        }
        // Mark that heartbeat was sent
        markHeartbeatSent(timestamp);
    }
}

void HeartbeatManager::notifyMessageSent(unsigned long timestamp)
{
    m_last_message_time = timestamp;
}

bool HeartbeatManager::shouldSendHeartbeat(unsigned long timestamp) const
{
    // Check if enough time has passed since the last message
    return (timestamp - m_last_message_time) >= m_interval_ms;
}

void HeartbeatManager::markHeartbeatSent(unsigned long timestamp)
{
    m_last_message_time = timestamp;
}

} // namespace Heartbeat
