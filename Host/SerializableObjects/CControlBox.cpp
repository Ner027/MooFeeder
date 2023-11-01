#include <iostream>
#include <fstream>
#include "CControlBox.h"

CControlBox* CControlBox::m_instance = nullptr;

CControlBox *CControlBox::getInstance()
{
    if (m_instance == nullptr)
        m_instance = new CControlBox;

    return m_instance;
}

void CControlBox::killInstance()
{
    delete m_instance;
    m_instance = nullptr;
}

bool CControlBox::loadFromJson(std::string& jsonString)
{
    json jObject = json::parse(jsonString);

    if (!jObject.contains("name") || !jObject.contains("username"))
        return false;

    m_name = jObject["name"];
    m_username = jObject["username"];

    //Session Token is optional
    if (jObject.contains("session_token"))
        m_sessionToken = jObject["session_token"];

    return true;
}

std::string CControlBox::dumpToJson()
{
    json jObject;

    jObject["name"] = m_name;
    jObject["username"] = m_username;
    jObject["session_token"] = m_sessionToken;

    return to_string(jObject);
}

bool CControlBox::dumpToFile(const std::string& filePath)
{
    std::ofstream file(filePath, std::ios::out | std::ios::trunc);

    if (!file.is_open())
        return false;

    file << dumpToJson() << std::endl;

    return true;
}

bool CControlBox::loadFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    std::string fileCtx;
    std::stringstream ss;

    if (!file.is_open())
        return false;

    ss << file.rdbuf();
    fileCtx = ss.str();

    return loadFromJson(fileCtx);
}

bool CControlBox::executeLogin(const std::string& username, const std::string& password)
{

}

CControlBox::CControlBox() = default;


