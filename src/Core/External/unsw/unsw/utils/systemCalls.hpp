#ifndef _SYSTEM_CALLS_HPP_
#define _SYSTEM_CALLS_HPP_

#include <cstdlib>

namespace systemCalls
{
    int _system(const char *cmd)
    {
        return system(cmd);
    }
}


#endif
