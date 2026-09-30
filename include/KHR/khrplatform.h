#ifndef __khrplatform_h_
#define __khrplatform_h_

#if defined(_WIN32) && !defined(__SC__)
#define KHRONOS_APICALL __declspec(dllimport)
#define KHRONOS_APIENTRY __stdcall
#else
#define KHRONOS_APICALL
#define KHRONOS_APIENTRY
#endif

#include <stddef.h>
#include <stdint.h>

typedef int32_t khronos_int32_t;
typedef uint32_t khronos_uint32_t;
typedef int64_t khronos_int64_t;
typedef uint64_t khronos_uint64_t;
typedef intptr_t khronos_intptr_t;
typedef uintptr_t khronos_uintptr_t;
typedef float khronos_float_t;

#endif
