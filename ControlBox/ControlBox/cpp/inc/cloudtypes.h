#ifndef CONTROLBOX_CLOUDTYPES_H
#define CONTROLBOX_CLOUDTYPES_H

#define CLOUD_URL "http://localhost:8080"

#define ENDPOINT_LOGIN "/box/login"
#define ENDPOINT_REGISTER "/box/register"

#define FIELD_USER "username"
#define FIELD_PSW "password"
#define FIELD_CAUSE "cause"
#define FIELD_RESERVED "reserved"
#define FIELD_TOKEN "session_token"

enum UserReturnCode_et
{
    INVALID_USER            = 0,
    INVALID_PASSWORD        = 1,
    USER_EXISTS             = 2,
    USER_NOT_FOUND          = 3,
    WRONG_PASSWORD          = 4,
    USER_OK                 = 5,
    INTERNAL_ERROR          = 6
};

#endif
