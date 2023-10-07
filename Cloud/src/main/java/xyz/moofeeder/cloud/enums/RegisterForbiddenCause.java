package xyz.moofeeder.cloud.enums;

public enum RegisterForbiddenCause
{
    INVALID_USER        (0),
    INVALID_PASSWORD    (1),
    USER_EXISTS    (2);

    private final int m_value;
    RegisterForbiddenCause(int value)
    {
        m_value = value;
    }

    public int getValue()
    {
        return m_value;
    }
}
