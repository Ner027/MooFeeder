package xyz.moofeeder.cloud.entities;

import org.json.JSONArray;
import xyz.moofeeder.cloud.enums.SerializableFieldType;
import xyz.moofeeder.cloud.util.Consts;
import xyz.moofeeder.cloud.util.Util;

public class Calf extends SerializableObject
{
    @SerializableField(name = "calf_id", type = SerializableFieldType.ENCODE)
    long m_calfId;
    @SerializableField(name = "parent_id", type = SerializableFieldType.ENC_DEC)
    long m_parentId;
    @JsonField(name = "notes")
    @SerializableField(name = "notes", type = SerializableFieldType.ENC_DEC, encode = true)
    String m_notes;
    @JsonField(name = "phyTag")
    @SerializableField(name = "phy_tag", type = SerializableFieldType.ENC_DEC, encode = true)
    String m_phyTag;
    @JsonField(name = "maxConsumption")
    @SerializableField(name = "max_consumption", type = SerializableFieldType.ENC_DEC, encode = false)
    double m_maxConsumption;

    @JsonField(name = "currentConsumption")
    double m_currentConsumption;
    public Calf()
    {
        m_parentId = -1;
        m_calfId = -1;
        m_maxConsumption = Consts.DEFAULT_MAX_CONSUMPTION;
        m_currentConsumption = 0;
    }
    public boolean setNotes(String newNotes)
    {
        return false;
    }

    public boolean setParent(long newParentId)
    {
        if (newParentId < 0)
            return false;

        m_parentId = newParentId;

        return true;
    }

    public boolean setPhyTag(String newTag)
    {
        if (newTag == null)
            return false;

        if (newTag.length() != Consts.PHY_ID_LEN)
            return false;

        m_phyTag = newTag;

        return true;
    }

    public void recalculateConsumption(int timestamp)
    {
        JSONArray logs = Util.getCalfLogsById(m_calfId, timestamp);
        m_currentConsumption = Util.calculateConsumptionFromLogs(logs);
    }

    public long getId()
    {
        return m_calfId;
    }
    public long getParentId()
    {
        return m_parentId;
    }

    @Override
    public String getInsertionQueryName()
    {
        return "InsertCalf";
    }

    @Override
    public String getUpdateQueryName()
    {
        return null;
    }

    @Override
    public String getLoadQueryName(String fieldName)
    {
        return "GetCalfBy-" + fieldName;
    }

    @Override
    public String getDeleteQueryName(String fieldName)
    {
        return null;
    }
}
