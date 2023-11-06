package xyz.moofeeder.cloud.entities;

import kotlin.Pair;
import xyz.moofeeder.cloud.enums.SerializableFieldType;
import xyz.moofeeder.cloud.security.Encryption;
import xyz.moofeeder.cloud.util.Consts;
import xyz.moofeeder.cloud.util.Util;

import java.sql.SQLException;

public class ControlBox extends SerializableObject
{
    @SerializableField(name = "id", type = SerializableFieldType.ENCODE)
    private final long m_id;
    @SerializableField(name = "session_token", type = SerializableFieldType.ENC_DEC)
    private String m_sessionToken;
    @SerializableField(name = "name", type = SerializableFieldType.ENC_DEC)
    private String m_name;
    @SerializableField(name = "password_salt", type = SerializableFieldType.ENC_DEC, encode = true)
    private String m_passwordSalt;
    @SerializableField(name = "username", type = SerializableFieldType.ENC_DEC, encode = true)
    private String m_username;
    @SerializableField(name = "password", type = SerializableFieldType.ENC_DEC, encode = true)
    private String m_password;

    public ControlBox() { m_id = -1; }

    public String getSessionToken()
    {
        return m_sessionToken;
    }

    public String refreshSessionToken() throws SQLException, IllegalAccessException
    {
        m_sessionToken = Util.generateRandomString(Consts.sessionTokenLen);
        update();
        return m_sessionToken;
    }

    public long getId()
    {
        return m_id;
    }

    public String getName()
{
        return m_name;
    }

    public String getUsername()
    {
        return m_username;
    }

    public boolean setUsername(String username)
    {
        if ((username.length() < 4) || (username.length() > 16))
            return false;

        m_username = username;

        return true;
    }

    public boolean setPassword(String password)
    {
        if ((password.length() < 8) || (password.length() > 32))
            return false;

        Pair<String, String> encodedPassword = Encryption.encodePassword(password);

        m_passwordSalt = encodedPassword.getFirst();
        m_password = encodedPassword.getSecond();

        return true;
    }

    public boolean checkAccess(String rawPassword)
    {
        if (m_id < 0)
            return false;

        return Encryption.checkPassword(rawPassword, m_passwordSalt, m_password);
    }


    @Override
    public String getInsertionQueryName()
    {
        return "InsertControlBox";
    }

    @Override
    public String getUpdateQueryName()
    {
        return "UpdateControlBox";
    }

    @Override
    public String getLoadQueryName(String fieldName)
    {
        return "GetControlBoxBy_" + fieldName;
    }

    @Override
    public String getDeleteQueryName(String fieldName)
    {
        return null;
    }
}
