package xyz.moofeeder.cloud.enums;

public enum AuthErrorCause
{
    INVALID_TOKEN        (0),
    TOKEN_EXPIRED        (1);

    private final int m_value;
    AuthErrorCause(int value)
    {
        m_value = value;
    }

    public int getValue()
    {
        return m_value;
    }
}
