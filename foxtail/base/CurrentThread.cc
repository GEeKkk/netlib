#include "foxtail/base/CurrentThread.h"

#include <type_traits>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

namespace CurrentThread
{
    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char *t_threadName = "unknown";
    static_assert(std::is_same<int, pid_t>::value, "pid_t should be int");

    pid_t GetTid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }

    void CacheTid()
    {
        if (t_cachedTid == 0)
        {
            // t_cachedTid = GetTid();
            t_cachedTid = gettid();
            t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
        }
    }
    bool IsMainThread()
    {
        return tid() == ::getpid();
    }

} // namespace CurrentThread
