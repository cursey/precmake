#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include "utl/mem.h"

// posix
void *mem_reserve(usize size)
{
    void *result = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return result == MAP_FAILED ? 0 : result;
}

bool mem_commit(void *buf, usize size)
{
    usize aligned_size = (size + 0xFFFF) & ~0xFFFF;
    if (mprotect(buf, aligned_size, PROT_READ | PROT_WRITE) == -1)
    {
        return false;
    }
    return true;
}

void mem_decommit(void *buf, usize size)
{
    usize aligned_size = (size + 0xFFFF) & ~0xFFFF;
    madvise(buf, aligned_size, MADV_DONTNEED);
    mprotect(buf, aligned_size, PROT_NONE);
}

void mem_release(void *buf, usize size)
{
    munmap(buf, size);
}