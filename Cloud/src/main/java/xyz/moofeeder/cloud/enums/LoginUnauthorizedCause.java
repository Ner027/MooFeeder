package xyz.moofeeder.cloud.enums;

public enum LoginUnauthorizedCause
{
    USER_NOT_FOUND        (0),
    INVALID_USER          (1),
    INVALID_PASSWORD      (2),
    WRONG_PASSWORD        (3);

    private final int m_value;
    LoginUnauthorizedCause(int value)
    {
        m_value = value;
    }

    public int getValue()
    {
        return m_value;
    }
}
