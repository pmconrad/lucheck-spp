#ifndef BYTE_H
#define BYTE_H

#include <string.h>

#define byte_copy(to,n,from)   memcpy(to,from,n)
#define byte_copyr(to,n,from)  memmove(to,from,n)
#define byte_diff(s,n,t)       memcmp(s,t,n)
#define byte_zero(s,n)         memset(s,0,n)

#define byte_equal(s,n,t) (!byte_diff((s),(n),(t)))

#endif
