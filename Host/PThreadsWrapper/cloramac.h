#ifndef PTHREADSWRAPPER_CLORAMAC_H
#define PTHREADSWRAPPER_CLORAMAC_H


#include "cthread.h"

class CLoRaMac : public CThread
{
    void * run(void *) override;
};


#endif
