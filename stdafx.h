#ifndef H_DEFINES
#define H_DEFINES

#include "logging.hpp"

typedef unsigned int uint;
typedef unsigned long long uLL;

#define HIDWORD(a) ((uint)((uLL)(a) >> 32))
#define LODWORD(a) ((uint)((uLL)(a) & 0xffffffff))
#define COMBINE(a, b) (((uLL)(a) << 32) + (uLL)(b))

#endif //H_DEFINES