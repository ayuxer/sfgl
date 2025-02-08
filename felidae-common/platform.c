#include "felidae/common/platform.h"

#ifdef WIN32
#include <windows.h>
#else
#include <time.h>
#include <unistd.h>
#endif

void felidae_sleep(unsigned int time_ms)
{
#ifdef WIN32
    Sleep(time_ms);
#else
    struct timespec ts;
    ts.tv_sec = time_ms / 1000;
    ts.tv_nsec = (time_ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
#endif
}
