package xyz.moofeeder.cloud.entities;

public class Calf implements DBObject
{
    @Override
    public String getInsertionQueryName()
    {
        return null;
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
