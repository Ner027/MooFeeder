#include "../../inc/gui/cguimenu.h"
#include "../../inc/gui/cqmlinterface.h"

void CGuiMenu::goBack()
{
    CQmlInterface::getInstance()->goBack();
}

CGuiMenu::CGuiMenu()
{
    m_controlBoxInstance = CControlBox::getInstance();
}
