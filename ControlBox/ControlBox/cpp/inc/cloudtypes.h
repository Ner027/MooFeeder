#ifndef CONTROLBOX_CLOUDTYPES_H
#define CONTROLBOX_CLOUDTYPES_H

#define CLOUD_URL               "http://localhost:8017"

#define ENDPOINT_LOGIN          "/box/login"
#define ENDPOINT_REGISTER       "/box/register"

#define ENDPOINT_CALF_ADD       "/calf/add"
#define ENDPOINT_CALF_CHANGE    "/calf/change"
#define ENDPOINT_CALF_DATA      "/calf/data"
#define ENDPOINT_CALF_LIST      "/calf/list"

#define ENDPOINT_STATION_LIST   "/station/list"

#define FIELD_USER              "username"
#define FIELD_PSW               "password"
#define FIELD_CAUSE             "cause"
#define FIELD_RESERVED          "reserved"
#define FIELD_TOKEN             "sessionToken"
#define FIELD_PHY_TAG           "phyId"
#define FIELD_PARENT_STATION    "parentStation"
#define FIELD_CONSUMPTION       "consumption"

enum CloudReturnCode_et
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
