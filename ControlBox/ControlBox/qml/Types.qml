import QtQuick 2.15

Item
{
    enum GuiMenuType
    {
        MAIN_MENU,
        MONITOR_MENU    = 0x01,
        STATION_MENU    = 0x02,
        BOX_MENU        = 0x03,
        SETTINGS_MENU   = 0x04,
        LOGIN_MENU      = 0x05
    }

    enum LoginReturnCode
    {
        INVALID_USER            = 0,
        INVALID_PASSWORD        = 1,
        USER_EXISTS             = 2,
        USER_NOT_FOUND          = 3,
        WRONG_PASSWORD          = 4,
        USER_LOGGED             = 5,
        INTERNAL_ERROR          = 6
    }
}
