#ifndef COMMON_RESOURCE_H
#define COMMON_RESOURCE_H

#include <stdint.h>
#include <stdlib.h>

#if defined(_MSC_VER) && defined(MEMORY_LEAK_DETECT)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(MEMORY_LEAK_DETECT)
#define u_allocate(s) u_allocate_dbg(__FILE__, __LINE__, (s))
#define u_realloc(p, s) u_realloc_dbg(__FILE__, __LINE__, (p), (s))
#define u_free(p) u_free_dbg(__FILE__, __LINE__, (p))

void *u_allocate_dbg(const char *file_name, int line, size_t size);

void *u_realloc_dbg(const char *file_name, int line, void *p_memory, size_t size);

void u_free_dbg(const char *file_name, int line, void **p_memory);

#else
void *u_allocate(size_t size);
void *u_realloc(void *p_memory, size_t size);
void  u_free(void **p_memory);
#endif

void pick_process_statistic();

#if defined(__cplusplus)
}
#endif

#endif //PINOCCHIO2_CORE_UTIL_MEMORY_H
