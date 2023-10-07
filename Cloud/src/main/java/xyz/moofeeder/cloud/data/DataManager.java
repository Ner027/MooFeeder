package xyz.moofeeder.cloud.data;

import java.sql.*;
import java.util.logging.Level;
import java.util.logging.Logger;

import kotlin.Pair;
import xyz.moofeeder.cloud.entities.ControlBox;
import xyz.moofeeder.cloud.util.QueryUtil;

public class DataManager
{
    private static Connection m_connection;
    private static final Logger m_logger = Logger.getLogger(DataManager.class.getName());

    public static void initDatabases()
    {
        insertData("InitDatabase");
    }

    private static Connection getConnection()
    {
        if (m_connection == null)
        {
            SettingsManager settings = SettingsManager.getInstance();
            try
            {
                m_connection = DriverManager.getConnection(
                        settings.getProperty("postgres_url"),
                        settings.getProperty("postgres_user"),
                        settings.getProperty("postgres_password"));
            }
            catch (SQLException e)
            {
                m_logger.log(Level.SEVERE, "Unable to connect to database!", e);
                System.exit(-1);
            }
        }

        return m_connection;
    }

    public static PreparedStatement getRawPreparedStatement(String qName) throws SQLException
    {
        return getConnection().prepareStatement(QueryUtil.getQuery(qName));
    }
    private static PreparedStatement prepareStatement(String qName, Object... params) throws SQLException
    {
        PreparedStatement pStat = getConnection().prepareStatement(QueryUtil.getQuery(qName));

        for (int i = 0; i < params.length; i++)
        {
            pStat.setObject(i + 1, params[i]);
        }

        return pStat;
    }



    @SuppressWarnings("unchecked")
    public static <T> T getData(Class<T> type, String qName, String colName, Object... params)
    {
        try
        {
            PreparedStatement pStat = prepareStatement(qName, params);

            ResultSet set = pStat.executeQuery();

            while (set.next())
            {
                Object obj = set.getObject(colName);

                if (type.isInstance(obj))
                    return (T) obj;
            }
        }
        catch (SQLException e)
        {
            m_logger.log(Level.WARNING, "Unable to execute query!", e);
        }

        return null;
    }
    public static boolean insertData(String queryName, Object... params)
    {
        try
        {
            PreparedStatement pStat = prepareStatement(queryName, params);
            return pStat.execute();
        }
        catch (SQLException e)
        {
            m_logger.log(Level.WARNING, "Unable to execute an SQL Request!", e);
            return false;
        }
    }
}
