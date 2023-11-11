package xyz.moofeeder.cloud.enums;

public enum RequestErrorCause
{
    INVALID_USER            (0),
    INVALID_PASSWORD        (1),
    USER_EXISTS             (2),
    USER_NOT_FOUND          (3),
    WRONG_PASSWORD          (4),
    INVALID_TOKEN           (5),
    TOKEN_EXPIRED           (6),
    INVALID_ST_NAME         (7),
    INVALID_HWID            (8),
    STATION_NOT_FOUND       (9),
    STATION_EXISTS          (10),
    INVALID_PHY_ID          (11),
    CALF_EXISTS             (12);

    private final int m_value;
    RequestErrorCause(int value)
    {
        m_value = value;
    }

    public int getValue()
    {
        return m_value;
    }
}
