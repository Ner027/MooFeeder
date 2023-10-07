package xyz.moofeeder.cloud.entities;

public interface DBObject
{
    String getInsertionQueryName();
    String getUpdateQueryName();
    String getLoadQueryName(String fieldName);
}
