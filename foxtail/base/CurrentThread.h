#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

namespace CurrentThread
{
    // internal
    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char *t_threadName;
    void CacheTid();

    inline int tid()
    {
        if (__builtin_expect(t_cachedTid == 0, 0))
        {
            CacheTid();
        }
        return t_cachedTid;
    }

    inline const char *tidString() // for logging
    {
        return t_tidString;
    }

    inline int tidStringLength() // for logging
    {
        return t_tidStringLength;
    }

    inline const char *name()
    {
        return t_threadName;
    }

    bool IsMainThread();

    // void sleepUsec(int64_t usec); // for testing

    // string stackTrace(bool demangle);
} // namespace CurrentThread


#endif // MUDUO_BASE_CURRENTTHREAD_H