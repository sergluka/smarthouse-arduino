#include "MsTimer2.h"

namespace MsTimer2 {

void set(unsigned long ms, void (*fn)())
{
    MsTimerMock::mock().set(ms, fn);
}

void start()
{
    MsTimerMock::mock().start();
}

void stop()
{
    MsTimerMock::mock().stop();
}

}
