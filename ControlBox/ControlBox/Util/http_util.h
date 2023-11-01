#ifndef HTTP_UTIL_H
#define HTTP_UTIL_H

#include <QHttpMultiPart>

class CHttpForm
{
private:
    std::vector<QHttpPart*> m_allocatedParts;
    QHttpMultiPart* m_multiPart;
public:
    CHttpForm();
    ~CHttpForm();
    CHttpForm& addField(std::string fieldName, QByteArray fieldValue);
    QHttpMultiPart* getMultiPart();
};


#endif
