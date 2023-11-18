#include "../../inc/entities/clog.h"

CLog::CLog(float volume, int timestamp)
{
    m_consumedVolume = volume;
    m_timestamp = timestamp;
}

float CLog::getConsumedVolume()
{
    return m_consumedVolume;
}

int CLog::getTimestamp()
{
    return m_timestamp;
}
