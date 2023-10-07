package xyz.moofeeder.cloud.entities;

import xyz.moofeeder.cloud.enums.SerializableFieldType;

public class FeedingStation extends SerializableObject
{
    @SerializableField(name = "id", type = SerializableFieldType.ENCODE)
    private final String m_id;

    @SerializableField(name = "name", type = SerializableFieldType.ENC_DEC, encode = true)
    private String m_name;

    @SerializableField(name = "description", type = SerializableFieldType.ENC_DEC, encode = true)
    private String m_description;
    @SerializableField(name = "hw_id", type = SerializableFieldType.ENC_DEC, encode = true)
    private String m_hwId;
    @SerializableField(name = "parent_id", type = SerializableFieldType.ENC_DEC)
    private long m_parent;
    @SerializableField(name = "nof_calves", type = SerializableFieldType.ENC_DEC)
    private int m_nofCalves;

    public FeedingStation()
    {
        m_id = null;
    }

    @Override
    public String getInsertionQueryName()
    {
        return "InsertFeedingStation";
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
}
