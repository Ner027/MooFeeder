#ifndef CB_CSERIALIZABLEOBJECT_H
#define CB_CSERIALIZABLEOBJECT_H

#include <string>

class CSerializableObject
{
protected:
    virtual bool loadFromJson(std::string&) = 0;
    virtual std::string dumpToJson() = 0;
};

#endif
