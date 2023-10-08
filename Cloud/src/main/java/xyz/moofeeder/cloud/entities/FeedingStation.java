package xyz.moofeeder.cloud.entities;

import xyz.moofeeder.cloud.enums.SerializableFieldType;
import xyz.moofeeder.cloud.util.Consts;

public class FeedingStation extends SerializableObject
{
    @SerializableField(name = "id", type = SerializableFieldType.ENCODE)
    private final long m_id;
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
        m_id = -1;
    }

    public boolean setName(String name)
    {
        if (name == null)
            return false;

        if (name.equals(m_name))
            return false;

        if ((name.length() < 4) || name.length() > 32)
             return false;

        this.m_name = name;

        return true;
    }

    public boolean setDescription(String desc)
    {
        if (desc == null)
            return false;

        if (desc.equals(m_description))
            return false;

        if (desc.length() > 64)
            return false;

        this.m_description = desc;

        return true;
    }

    public boolean setParentId(long parentId)
    {
        if (parentId < 0)
            return false;

        m_parent = parentId;

        return true;
    }

    public boolean setHwId(String hwId)
    {
        if (hwId.length() != Consts.hwIdLen)
            return false;

        this.m_hwId = hwId;

        return true;
    }

    public long getId()
    {
        return m_id;
    }
    @Override
    public String getInsertionQueryName()
    {
        return "InsertFeedingStation";
    }
    @Override
    public String getUpdateQueryName()
    {
        return "UpdateFeedingStation";
    }

    @Override
    public String getLoadQueryName(String fieldName)
    {
        return "GetFeedingStationBy_" + fieldName;
    }

    @Override
    public String getDeleteQueryName(String fieldName)
    {
        return "DeleteFeedingStationBy_" + fieldName;
    }

}
