package xyz.moofeeder.cloud.enums;

public enum CreateForbiddenCause
{
    INVALID_NAME    (0),
    INVALID_HWID    (1);

    private final int m_value;
    CreateForbiddenCause(int value)
    {
        m_value = value;
    }

    public int getValue()
    {
        return m_value;
    }
}
