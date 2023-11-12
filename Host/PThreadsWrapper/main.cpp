#include "cmutex.h"
#include "cloramac.h"
#include <chrono>

int main()
{
    CMutex mutex;
    CLoRaMac loraMac;
    loraMac.join();

    auto deltaTime = std::chrono::system_clock::now() + std::chrono::milliseconds(1000);
    std::chrono::duration_cast<long, std::

    return 0;
}
