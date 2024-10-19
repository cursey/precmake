#include <Windows.h>
#include "utl/mem.h"

void *mem_reserve(usize size)
{
    return VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
}

bool mem_commit(void *buf, usize size)
{
    return VirtualAlloc(buf, size, MEM_COMMIT, PAGE_READWRITE) != 0;
}

void mem_decommit(void *buf, usize size)
{
    VirtualFree(buf, size, MEM_DECOMMIT);
}

void mem_release(void *buf, usize size)
{
    VirtualFree(buf, 0, MEM_RELEASE);
}