package xyz.moofeeder.cloud.entities;

import xyz.moofeeder.cloud.enums.SerializableFieldType;

public class Log extends SerializableObject
{
    @SerializableField(name = "log_id", type = SerializableFieldType.ENCODE)
    long m_logId;
    @SerializableField(name = "parent_id", type = SerializableFieldType.ENC_DEC)
    long m_parentId;
    @SerializableField(name = "log_timestamp", type = SerializableFieldType.ENC_DEC)
    long m_timestamp;
    @SerializableField(name = "consumed_volume", type = SerializableFieldType.ENC_DEC)
    double m_consumption;

    public Log(long parentId, double consumedVolume)
    {
        m_timestamp = System.currentTimeMillis() / 1000L;
        m_parentId = parentId;
        m_consumption = consumedVolume;
    }

    public Log()
    {
        m_timestamp = 0;
        m_parentId = -1;
        m_consumption = -1;
    }

    public long getTimestamp()
    {
        return m_timestamp;
    }

    public double getConsumption()
    {
        return m_consumption;
    }

    @Override
    public String getInsertionQueryName()
    {
        return "InsertLog";
    }

    @Override
    public String getUpdateQueryName()
    {
        return null;
    }

    @Override
    public String getLoadQueryName(String fieldName)
    {
        return null;
    }

    @Override
    public String getDeleteQueryName(String fieldName)
    {
        return null;
    }
}
