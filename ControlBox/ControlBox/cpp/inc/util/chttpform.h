#ifndef CONTROLBOX_CHTTPFORM_H
#define CONTROLBOX_CHTTPFORM_H

#include <QHttpPart>
#include <QHttpMultiPart>
#include <vector>

class CHttpForm
{
public:
    CHttpForm();
    ~CHttpForm();
    CHttpForm& addField(std::string fieldName, QByteArray fieldValue);
    [[nodiscard]] QHttpMultiPart* getMultiPart() const;
private:
    std::vector<QHttpPart*> m_allocatedParts;
    QHttpMultiPart* m_multiPart;
};


#endif
