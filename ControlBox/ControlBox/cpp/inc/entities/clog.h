#ifndef CONTROLBOX_CLOG_H
#define CONTROLBOX_CLOG_H


class CLog
{
public:
    CLog(float volume, int timestamp);
    CLog();
    float getConsumedVolume();
    int getTimestamp();
private:
    int m_timestamp;
    float m_consumedVolume;
};

#endif
