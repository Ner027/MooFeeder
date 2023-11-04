#include <iostream>
#include <thread>
#include "cloramac.h"

void *CLoRaMac::run(void *)
{
    while (1)
    {
        std::cout << "Hello from LoRa MAC" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
