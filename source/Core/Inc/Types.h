#ifndef TYPES_H_
#define TYPES_H_
#include <stdint.h>

// Used for temperature represented in C or x10C.
//

typedef int32_t TemperatureType_t;

#if (__GNUC__ > 14 || \
    (__GNUC__ == 14 && (__GNUC_MINOR__ >= 2 ))) && \
		(defined(__ELF__) && !defined(__LP64__))
typedef uint32_t u32_t;
typedef signed long s32_t;
typedef uint8_t u8_t;
#endif

#endif