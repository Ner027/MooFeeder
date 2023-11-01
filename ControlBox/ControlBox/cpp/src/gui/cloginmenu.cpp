#include "../../inc/gui/cloginmenu.h"
#include "../../inc/entities/ccontrolbox.h"

LoginReturnCode_et CLoginMenu::loginUser(QString &username, QString &password)
{
    return CControlBox::getInstance()->executeLogin(username.toStdString(), password.toStdString());
}
