package xyz.moofeeder.cloud.data;

import kotlin.Pair;
import xyz.moofeeder.cloud.util.QueryUtil;

import java.sql.PreparedStatement;
import java.sql.SQLException;
import java.util.HashMap;
import java.util.List;

public class MappedStatement
{
    private final PreparedStatement m_preparedStatement;
    private final HashMap<String, Integer> m_map = new HashMap<>();
    private boolean m_prepared = false;
    public MappedStatement(String qName) throws SQLException
    {
        int paramCount = 1;
        boolean isNamedParameter = false;
        String sqlQuery = QueryUtil.getQueryRaw(qName);
        StringBuilder parameterName = new StringBuilder();

        for (char c : sqlQuery.toCharArray())
        {
            if (c == '<')
            {
                isNamedParameter = true;
                parameterName.setLength(0);
                continue;
            }
            else if (c == '>')
            {
                isNamedParameter = false;
                m_map.put(parameterName.toString(), paramCount++);
            }

            if (!isNamedParameter)
                continue;

            parameterName.append(c);
        }

        //If isNamedParameter is true, it means one of the named parameters wasn't encapsulated properly
        if (isNamedParameter)
            throw new SQLException("Invalid named parameter query!");

        m_preparedStatement = DataManager.getRawPreparedStatement(qName);
    }

    public PreparedStatement getPreparedStatement(List<Pair<String, Object>> params) throws SQLException
    {
        if (!m_prepared)
        {
            for (Pair<String, Object> param : params)
            {
                String key = param.getFirst();

                if (!m_map.containsKey(key))
                    throw new SQLException("Parameter not found!");

                m_preparedStatement.setObject(m_map.get(key), param.getSecond());
            }
        }

        return m_preparedStatement;
    }
}
