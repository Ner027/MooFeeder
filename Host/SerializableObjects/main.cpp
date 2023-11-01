#include "CControlBox.h"
#include "http.hpp"

int main()
{
    httplib::Client client("http://localhost:8080/box");

    httplib::MultipartFormDataItems form =
    {
            {"username", "andre"},
            {"password", "mooFeeder"}
    };

    return 0;
}
