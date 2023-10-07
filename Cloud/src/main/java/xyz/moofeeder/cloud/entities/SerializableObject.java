package xyz.moofeeder.cloud.entities;

import kotlin.Pair;
import xyz.moofeeder.cloud.data.DataManager;
import xyz.moofeeder.cloud.data.MappedStatement;
import xyz.moofeeder.cloud.enums.SerializableFieldType;

import java.lang.reflect.Field;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Arrays;
import java.util.Base64;
import java.util.LinkedList;
import java.util.List;

public abstract class SerializableObject implements DBObject
{
    private List<Pair<String, Object>> getFields(boolean decOnly) throws IllegalAccessException
    {
        List<Pair<String, Object>> fieldList = new LinkedList<>();

        for (Field field : getClass().getDeclaredFields())
        {
            if (!field.isAnnotationPresent(SerializableField.class))
                continue;

            SerializableField sf = field.getAnnotationsByType(SerializableField.class)[0];

            if ((sf.type() == SerializableFieldType.ENCODE) && decOnly)
                continue;

            boolean isAccessible = field.canAccess(this);

            if (!isAccessible)
                field.setAccessible(true);

            Object obj = field.get(this);

            if ((field.getType() == String.class) && (sf.encode()) && (obj != null))
                fieldList.add(new Pair<>(sf.name(), new String(Base64.getEncoder().encode(obj.toString().getBytes()))));
            else
                fieldList.add(new Pair<>(sf.name(), obj));

            if (!isAccessible)
                field.setAccessible(false);
        }

        return fieldList;
    }
    public void insert() throws IllegalAccessException, SQLException
    {
        MappedStatement mStat = new MappedStatement(getInsertionQueryName());
        mStat.getPreparedStatement(getFields(true)).execute();
    }
    public void update() throws SQLException, IllegalAccessException
    {
        MappedStatement mStat = new MappedStatement(getUpdateQueryName());
        mStat.getPreparedStatement(getFields(false)).execute();
    }

    private boolean isFieldEncoded(String fieldName)
    {
        for (Field field : getClass().getDeclaredFields())
        {
            if (!field.isAnnotationPresent(SerializableField.class))
                continue;

            SerializableField sf = field.getAnnotationsByType(SerializableField.class)[0];

            if (sf.name().equals(fieldName))
                return sf.encode();
        }

        return false;
    }

    public boolean load(String fieldName, Object id) throws SQLException, IllegalAccessException
    {
        int serializableFields = 0;

        PreparedStatement pStat = DataManager.getRawPreparedStatement(getLoadQueryName(fieldName));

        if (isFieldEncoded(fieldName))
            pStat.setObject(1, Base64.getEncoder().encodeToString(id.toString().getBytes()));
        else
            pStat.setObject(1, id);

        ResultSet set = pStat.executeQuery();

        if (!set.next())
            return false;

        for (Field field : getClass().getDeclaredFields())
        {
            if (!field.isAnnotationPresent(SerializableField.class))
                continue;

            serializableFields++;
            SerializableField sf = field.getAnnotationsByType(SerializableField.class)[0];

            Object obj = set.getObject(sf.name());

            if (obj == null)
                continue;

            boolean isAccessible = field.canAccess(this);

            if (!isAccessible)
                field.setAccessible(true);

            if ((field.getType() == String.class) && (sf.encode()))
                field.set(this, new String(Base64.getDecoder().decode(obj.toString())));
            else
                field.set(this, obj);

            if (!isAccessible)
                field.setAccessible(false);

            serializableFields--;
        }

        return serializableFields == 0;
    }
}
