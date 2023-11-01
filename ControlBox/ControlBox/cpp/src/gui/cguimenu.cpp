#include "../../inc/gui/cguimenu.h"
#include "../../inc/gui/cqmlinterface.h"

void CGuiMenu::goBack()
{
    CQmlInterface::getInstance()->goBack();
}
