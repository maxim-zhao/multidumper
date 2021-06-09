#include "timefn.h"

#include <time.h>

uint64_t dostime_to_timestamp(unsigned long t)
{
    uint64_t epoch = ((uint64_t)0x019db1de << 32) + 0xd53e8000;
                     /* 0x019db1ded53e8000ULL: 1970-01-01 00:00:00 (UTC) */
	struct tm tm;

#define subbits(n, off, len) (((n) >> (off)) & ((1 << (len))-1))

    tm.tm_sec  = subbits(t,  0, 5) * 2;
    tm.tm_min  = subbits(t,  5, 6);
    tm.tm_hour = subbits(t, 11, 5);
    tm.tm_mday = subbits(t, 16, 5);
    tm.tm_mon  = subbits(t, 21, 4) - 1;
    tm.tm_year = subbits(t, 25, 7) + 80;
    tm.tm_isdst = -1;

    return ( mktime(&tm) * 1000000 ) + epoch;
}
