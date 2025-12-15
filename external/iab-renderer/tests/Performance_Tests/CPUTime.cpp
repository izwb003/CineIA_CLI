/*======================================================================*
    Copyright (c) 2015-2023 DTS, Inc. and its affiliates.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software without
    specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *======================================================================*/

#include "CPUTime.h"

#ifdef _WIN32

#include <Windows.h>

inline unsigned long long filetime2ll(FILETIME filetime)
{
    return ((unsigned long long)filetime.dwHighDateTime) << 32 | (unsigned long long) filetime.dwLowDateTime;
}

/**
 * Returns the total CPU time of the process in nanoseconds since its start.
 * The CPU time includes user and kernel time.
 *
 */
void getCPUTime(_cpu_time *t)
{
    FILETIME ct;
    FILETIME et;
    FILETIME ktime;
    FILETIME utime;
    
    GetProcessTimes(GetCurrentProcess(), &ct, &et, &ktime, &utime);
    
    *t = (filetime2ll(ktime) + filetime2ll(utime)) * 100;
}

double diffCPUTime(_cpu_time *end, _cpu_time *start)
{
    return (double) (*end - *start);
}

#elif __APPLE__

#include <time.h>
#include <sys/resource.h>

inline double diff_timeval(timeval *end, timeval *start)
{
    if (end->tv_usec < start->tv_usec)
    {
        return (start->tv_usec - end->tv_usec) * 1e3 +  (difftime(end->tv_sec, start->tv_sec) - 1) * 1e9;
    }
    else
    {
        return (end->tv_usec - start->tv_usec) * 1e3 +  difftime(end->tv_sec, start->tv_sec) * 1e9;
    }
}

double diffCPUTime(_cpu_time *end, _cpu_time *start)
{
    return diff_timeval(&(end->ru_utime), &(start->ru_utime)) +
    diff_timeval(&(end->ru_stime), &(start->ru_stime));
}

void getCPUTime(_cpu_time *t)
{
    getrusage(RUSAGE_SELF, t);
}

#else

inline double diff_timeval(timespec *end, timespec *start)
{
    if (end->tv_nsec < start->tv_nsec)
    {
        return (start->tv_nsec - end->tv_nsec) +  (difftime(end->tv_sec, start->tv_sec) - 1) * 1e9;
    }
    else
    {
        return (end->tv_nsec - start->tv_nsec) +  difftime(end->tv_sec, start->tv_sec) * 1e9;
    }
}

double diffCPUTime(_cpu_time *end, _cpu_time *start)
{
    return diff_timeval(end, start);
}

void getCPUTime(_cpu_time *t)
{
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, t);
}

#endif